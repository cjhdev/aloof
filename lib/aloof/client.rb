require 'digest/crc16_ccitt'
require 'logger'

module Aloof

  class Client
  
    attr_reader :defs, :timeout, :log
    
    def next_invoke_id
      value = @invoke_id
      @invoke_id += 1
      value
    end
  
    def initialize(**opts)
    
      @log = Logger.new(STDOUT)
    
      @invoke_id=0
    
      @defs = opts[:registers]||{}
      
      @timeout = opts[:timeout]||5
      
      @defs = @defs.map do |k,v|
        
        raise unless v.kind_of? Hash
        
        value = v.dup
        
        case v[:id]
        when String
          value[:id] = v[:id].to_i(16)
        when Integer
          value[:id] = v[:id]
        else
          raise "id '#{v[:id]}' is invalid"
        end
        
        [
          k,
          value
        ]
        
      end.to_h
        
      @transport = Slip.new(**opts)
      
      decoder = MessageDecoder.new
    
      @pending = {}
    
      @transport.on_rx do |frame|
      
        crc = Digest::CRC16CCITT.new
        crc << frame
        
        if crc.checksum == 0
        
          begin
            msg = decoder.decode(frame.slice(0..-3))
          rescue => e
            puts e
          end
        
          case msg
          when Message::Response          
            if job = @pending[msg.invoke_id]
              job.push(msg)
            end          
          end
        
        else
        
          puts "discarding bad CRC"
        
        end
        
      end
    
    end
    
    def open
      @transport.open
    end
    
    def close
      @transport.close
    end
    
    def read(oid, **opts)
    
      definition = defs[oid.to_sym]
      
      raise RangeError.new "#{oif} is an unknown object" unless definition
    
      confirmed = opts[:confirmed]||true
    
      cmd = Message::Command.new(confirmed, next_invoke_id, Message::ReadCommand.new(definition[:id]))
      
      resp = transport(cmd)
      
      case resp
      when Message::Response 
        case resp.type
        when Message::ReadResponse
          if resp.type.result == :success
          
            value = resp.type.value
          
            if opts[:from_terminal]
            
              case definition[:type]
              when "blob"
                value = value.bytes.map{|b|"%02X"%b}.join                
              else
                value = value.to_s
              end
            
            end
          
            return resp.type.result, resp.type.type, value
            
          else
            return resp.type.result, resp.type.reason
          end
        else
          raise "unexpected response type"
        end
      when Message::Error
        raise "device reports error"
      else
        raise TimeoutError          
      end
    
    end
    
    def write(oid, value, **opts)        
      
      definition = defs[oid]
      
      raise RangeError.new "#{oid} is an unknown object" unless definition
      
      confirmed = opts[:confirmed]||true
      
      # convert value from terminal
      if opts[:from_terminal]
      
        case definition[:type]
        when "bool"
          case value.downcase
          when "true"
            value = true
          when "false"  
            value = false
          else
            raise "invalid input"
          end
        when "u8","u16","u32","u64",
          value = value.to_i
        when "float", "double"
          value = value.to_f
        when "blob"
          input = input.pack("H*")
        end
        
      end
      
      cmd = Message::Command.new(confirmed, next_invoke_id, Message::WriteCommand.new(definition[:id], definition[:type].to_sym, value))
      
      resp = transport(cmd)
      
      case rsp
      when Message::Response      
      when Message::Error
      else
        raise
      end
      
    end
    
    def transport(msg)
    
      frame = msg.encode
      
      crc = Digest::CRC16CCITT.new
      crc << frame
      frame << [crc.checksum].pack("S>")
      
      @transport.tx(frame)
    
      if msg.confirmed?
      
        q = TimeoutQueue.new
        
        @pending[msg.invoke_id] = q
      
        begin
          response = q.pop(timeout: timeout)
        rescue ThreadError
          response = nil
        end
        
        @pending.delete(msg.invoke_id)
        
        response
    
      end
        
    end
    
  end

end
