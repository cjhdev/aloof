require 'serialport'
require 'digest/crc16_ccitt'

module Aloof

  class Slip
  
    END_CHAR = 192
    ESCAPE_CHAR = 219
    ESCAPE_END_CHAR = 220
    ESCAPE_ESCAPE_CHAR = 221
  
    def self.open(**opts, &block)
      framer = self.new(**opts)
      framer.open
      block.call(framer)
      framer.close
      self      
    end
  
    def initialize(**opts)    
      @port = nil
      @dev = opts[:dev]
      @baud = opts[:baud]||9600
      @running = false      
      @rx_handler = nil
      @reader = nil    
      @opts = opts
      @transport = opts[:transport]||:slip
    end
  
    def open?
      @running
    end
    
    def tx(buffer)
    
      case @transport
      when :slip_crc16
        crc = Digest::CRC16CCITT.new
        crc << buffer
        buffer << [crc.checksum].pack("S>")
      end
      
      puts "tx: #{buffer.bytes.map{|b|"%02X" % b}.join}"
    
      @port.write [END_CHAR].pack("C")
      buffer.bytes.each do |c|
        case c
        when END_CHAR
          @port.write [ESCAPE_CHAR,ESCAPE_END_CHAR].pack("CC")
        when ESCAPE_CHAR
          @port.write [ESCAPE_CHAR,ESCAPE_ESCAPE_CHAR].pack("CC")
        else
          @port.write [c].pack("C")
        end          
      end
      @port.write [END_CHAR].pack("C")
    end
    
    def on_rx(&handler)
      @rx_handler = handler
      self
    end    
    
    def open
      @port = SerialPort.new(@dev, @baud)
      @port.read_timeout = 0
      @port.flow_control = SerialPort::NONE
      @running = true      
      @reader = Thread.new do      
        buffer = []
        state = :rx
        @port.flush_input        
        begin        
          loop do              
            
            c = @port.getbyte
            
            case state
            when :rx              
              case c
              when ESCAPE_CHAR
                state = :escape              
              when END_CHAR        
              
                puts "rx: #{buffer.map{|b|"%02X" % b}.join}"
                
                packed = buffer.pack("C*")
                
                case @transport
                when :slip_crc16
                  
                  crc = Digest::CRC16CCITT.new
                  crc << packed
                  
                  if crc.checksum == 0
                  
                    packed.slice!(-3..-1)
                  
                    @rx_handler.call(packed) if @rx_handler and buffer.size > 0
                    
                  end
                  
                else
                
                  @rx_handler.call(packed) if @rx_handler and buffer.size > 0
                  
                end
                
                state = :rx
                buffer = []
              else
                buffer << c
              end                  
            when :escape           
              case c
              when ESCAPE_ESCAPE_CHAR
                buffer << ESCAPE_CHAR
                state = :rx      
              when ESCAPE_END_CHAR
                buffer << END_CHAR
                state = :rx      
              when ESCAPE_CHAR
                state = :escape
              else
                buffer << c
                state = :rx      
              end        
            end                
          end            
        rescue IOError, Interrupt
        end          
      end
    end
    
    def close
      if self.open?
        @reader.raise Interrupt
        @reader.join
        @running = false        
        @port.close
      end
      self    
    end
  
  end

end
