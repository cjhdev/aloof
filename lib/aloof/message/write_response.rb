module Aloof::Message

  class WriteResponse
  
    def self.tag
      0
    end
    
    def tag
      self.class.send(__method__)
    end
  
    def self.decode(buf)        
      result, reason = Base.get_access_result(buf)      
      self.new(result, reason)      
    end
  
    attr_reader :result, :reason
  
    def initialize(result, reason=nil)
      @result = result
      @reason = reason
    end
    
    def encode(buf=Aloof::OutputCodec.new)
      buf.put_uint(tag)
      put_access_result(buf, result, reason)    
      buf.buffer
    end
  
  end

end
