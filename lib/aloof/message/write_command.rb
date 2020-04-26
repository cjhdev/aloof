module Aloof::Message

  class WriteCommand
  
    def self.tag
      0
    end
    
    def tag
      self.class.send(__method__)
    end
  
    def self.decode(buf)
      
      oid = buf.get_uint
      type, value = Base.get_value(buf)      
      
      self.new(oid, type, value)
      
    end
  
    attr_reader :oid, :type, :value
  
    def initialize(oid, type, value)
      @oid = oid
      @type = type
      @value = value
    end
    
    def encode(buf=Aloof::OutputCodec.new)      
      buf.put_uint(tag)
      buf.put_uint(oid)
      Base.put_value(buf, type, value)      
      buf.buffer        
    end
  
  end

end
