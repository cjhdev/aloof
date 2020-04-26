module Aloof::Message

  class Command < Base

    require_relative "read_command"
    require_relative "write_command"
  
    COMMANDS = [
      WriteCommand,
      ReadCommand        
    ]
  
    def self.tag
      3
    end
    
    def tag
      self.class.send(__method__)
    end
    
    def self.decode(buf)
    
      invoke_field = buf.get_u8
      
      invoke_id = invoke_field & 0x7f
      
      confirmed = (invoke_field & 0x80) != 0 
      
      cls = COMMANDS[buf.get_uint]
      
      raise EncodingError unless cls
        
      self.new(confirmed, invoke_id, cls.decode(buf), **opts) if cmd
        
    end
  
    attr_reader :confirmed, :invoke_id, :type
  
    def confirmed?
      @confirmed
    end
    
    def initialize(confirmed, invoke_id, type)
      @confirmed = confirmed
      @invoke_id = invoke_id
      @type = type
    end
    
    def encode(buf=Aloof::OutputCodec.new)
    
      buf.put_uint(tag)
      
      invoke_field = invoke_id & 0x7f
      invoke_field |= confirmed ? 0x80 : 0x00
      
      buf.put_u8(invoke_field)
      
      type.encode(buf)
      
      buf.buffer
      
    end
  
  end

end
