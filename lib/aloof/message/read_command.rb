module Aloof::Message

  class ReadCommand
  
    def self.tag
      1
    end
    
    def tag
      self.class.send(__method__)
    end
  
    def self.decode(buf)
      
      oid = buf.get_uint      
      
      self.new(oid)
      
    end
  
    attr_reader :oid
  
    def initialize(oid)
      @oid = oid
    end
    
    def encode(buf=Aloof::OutputCodec.new)      
      buf.put_uint(tag)
      buf.put_uint(oid)
      buf.buffer        
    end
  
  end

end
