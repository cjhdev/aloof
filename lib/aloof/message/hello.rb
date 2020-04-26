
module Aloof::Message

  class Hello < Base
      
    def self.tag
      0
    end
    
    def tag
      self.class.send(__method__)
    end
    
    def self.decode(buf)    
    end
    
    attr_reader :version, :features, :station_type
    
    def initialize(version, features, station_type)
      @version = version
      @features = features
      @station_type = station_type
    end
    
    def encode(buf=Aloof::OutputCodec.new)
      
      buf.put_uint(tag)
      buf.put_uint(version)
      buf.put_u16(features)
      
      case station_type
      when :client
        buf.put_uint(0)
      when :server
        buf.put_uint(1)
      else
        raise
      end
      
      buf.buffer
      
    end
    
  end

end
