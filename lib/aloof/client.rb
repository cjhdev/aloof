require 'logger'

module Aloof

  class Client
  
    attr_reader :object_table, :timeout, :log
    
    def next_invoke_id
      value = @invoke_id
      @invoke_id += 1
      value
    end
  
    def on_alert(&block)
      @on_alert = block
      self
    end
  
    def self.open(**opts, &block)
      client = self.new(**opts)
      client.open
      block.call(client)
      client.close
      self
    end
  
    def initialize(**opts)
    
      @log = Logger.new(STDOUT)
    
      @invoke_id=0
    
      @timeout = opts[:timeout]||5
      
      @object_table = opts[:object_table]||ObjectTable.new
      
      @transport_type = opts[:transport]||:tcp
        
      case @transport_type
      when :slip, :slip_crc16
        @transport = Slip.new(**opts)
      when :tcp
        @transport = Tcp.new(**opts)
      else
        raise "unknown transport #{@transport_type}"
      end
      
      decoder = MessageDecoder.new
    
      @pending = {}
    
      @transport.on_rx do |frame|
        
        begin
          msg = decoder.decode(frame)
        rescue => e
          puts e
        end
      
        case msg
        when Message::Response          
          if job = @pending[msg.invoke_id]
            job.push(msg)
          end          
        when Message::Alert
          @on_alert.call(msg) if @on_alert
        end
        
      end
    
    end
    
    def open
      @transport.open
    end
    
    def close
      @transport.close
    end
    
    def read(name, **opts)
    
      obj = object_table.lookup_name(name)
      
      raise RangeError.new "#{name} is an unknown object" unless obj
    
      confirmed = opts[:confirmed]||true
    
      cmd = Message::Command.new(confirmed, next_invoke_id, 
        Message::ReadCommand.new(obj.oid)
      )
      
      resp = transport(cmd)
      
      case resp
      when Message::Response 
        
        case resp.type
        when Message::ReadResponse
        
          raise access_error_to_exception(resp.type.reason) if resp.type.result == :failure
          
          return resp.type.value
                    
        else
          raise "unexpected response type"
        end
        
      when Message::Error
        raise "device reports error"
      else
        raise TimeoutError          
      end
    
    end
    
    def write(name, value=nil, **opts)        
      
      obj = object_table.lookup_name(name)
      
      raise RangeError.new "#{name} is an unknown object" unless obj
      
      confirmed = opts[:confirmed]||true
      
      cmd = Message::Command.new(confirmed, next_invoke_id, 
        Message::WriteCommand.new(obj.oid, obj.type, value)
      )
      
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
    
    ACCESS_ERROR_TO_EXCEPTION = {
      unkown_oid: UnknownOID,
      access_denied: AccessDenied,
      authentication_required: AuthenticationRequired,
      argument_error: ArgumentError,
      temporary_failure: TemporaryFailure,
      application_error: ApplicationError
    }
    
    def access_error_to_exception(error)
      ACCESS_ERROR_TO_EXCEPTION[error]
    end
    
  end

end
