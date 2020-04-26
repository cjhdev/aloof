module Aloof::Message

  class Response < Base

    require_relative "read_response"
    require_relative "write_response"
  
    RESPONSES = [
      WriteResponse,
      ReadResponse
    ]
  
    def self.tag
      4
    end
    
    def tag
      self.class.send(__method__)
    end
    
    def confirmed?
      @confirmed
    end
    
    def self.decode(buf)
    
      invoke_field = buf.get_u8
      
      invoke_id = invoke_field & 0x7f
      
      confirmed = (invoke_field & 0x80) != 0 
      
      cls = RESPONSES[buf.get_uint]
      
      raise EncodingError unless cls
      
      self.new(confirmed, invoke_id, cls.decode(buf))
    
    end
    
    attr_reader :confirmed, :invoke_id, :type
    
    def initialize(confirmed, invoke_id, type)
      @confirmed = confirmed
      @invoke_id = invoke_id
      @type = type
    end
    
    def encode(buf=Aloof::OutputCodec.new)
    
      buf.put_u8(tag)
      
      invoke_field = invoke_id & 0x7f
      invoke_field |= confirmed ? 0x80 : 0x00
      
      buf.put_u8(invoke_field)
      
      type.encode(buf)
      
      buf.buffer
    
    end
  
  end

end
