module Aloof::Message

  class ReadResponse

    def self.tag
      1
    end
    
    def tag
      self.class.send(__method__)
    end

    def self.decode(buf)
      
      result, reason = Base.get_access_result(buf)
      
      type, value = Base.get_value(buf) if (result == :success)
      
      self.new(result, reason, type, value)
      
    end
  
    attr_reader :result, :reason, :type, :value
  
    def initialize(result, reason, type=nil, value=nil)
      @result = result
      @reason = reason
      @type = type
      @value = value
    end
    
    def encode(buf=Aloof::OutputCodec.new)        
      buf.put_uint(tag)        
      put_access_result(buf, result, reason)        
      Base.put_value(buf, type, value) if (result == :success)
      buf.buffer        
    end

  end

end
