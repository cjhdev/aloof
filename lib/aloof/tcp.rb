require 'socket'

module Aloof

  class Tcp
  
    def self.open(**opts, &block)
      framer = self.new(**opts)
      framer.open
      block.call(framer)
      framer.close
      self      
    end
  
    def initialize(**opts)    
      @socket = nil
      @host = opts[:host]||"localhost"
      @port = opts[:port]||4567
      @running = false      
      @rx_handler = nil
      @reader = nil    
      @opts = opts
    end
  
    def open?
      @running
    end
    
    def tx(buffer)
    
      puts "tx: #{buffer.bytes.map{|b|"%02X" % b}.join}"
    
      @socket.write [buffer.size, buffer].pack("S<a*")
      @socket.flush
      
    end
    
    def on_rx(&handler)
      @rx_handler = handler
      self
    end    
    
    def open
      @socket = TCPSocket.new @host, @port
      @running = true      
      @reader = Thread.new do      
        
        begin
        
          loop do
          
            buf = @socket.recv(2)
            
            size = buf.unpack("S<").first
            
            msg = @socket.recv(size)
            
            puts "rx: #{msg.bytes.map{|b|"%02X"%b}.join}"
            
            begin
              @rx_handler.call(msg) if @rx_handler and msg.size > 0
            rescue
            end
            
          end          
        end      
          
      rescue Interrupt
      rescue => e
        puts e
      end        
      
    end
    
    def close
      if self.open?
        @reader.raise Interrupt
        @reader.join
        @running = false        
        @socket.close
      end
      self    
    end
  
  end

end
