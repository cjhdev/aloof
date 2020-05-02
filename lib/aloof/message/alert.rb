module Aloof::Message

  class Alert < Base
  
    def self.tag
      5
    end
    
    def tag
      self.class.send(__method__)
    end
    
    def self.decode(buf)
    
      aid = buf.get_uint
    
      type, value = Base.get_value(buf)
      
      self.new(aid, type, value)
    
    end
    
    attr_reader :aid, :type, :value
    
    def initialize(aid, type, value)
      @aid = aid
      @type = type
      @value = value
    end
    
    def encode(buf=Aloof::OutputCodec.new)        
      buf.put_uint(tag)        
      buf.put_uint(aid)    
      Base.put_value(buf, type, value)
      buf.buffer        
    end
    
  end
  
end
