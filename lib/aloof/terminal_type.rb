module Aloof

  class TerminalType
    
    attr_reader :type
    
    def initialize(type)
      @type = type.to_sym
    end
    
    def to_terminal(value)
      
      case type
      when :blob
        return value.bytes.map{|b|"%02X"%b}.join    
      else
        return value.to_s
      end
    end
    
    def from_terminal(value)
    
      case type
      when :bool
        case value.downcase
        when "true"
          return true
        when "false"  
          return false
        else
          raise ::ArgumentError
        end
      when :u8, :u16, :u32, :u64, :i8, :i16, :i32, :i64
        return value.to_i        
      when :float, :double
        return value.to_f
      when :blob
        return value.pack("H*")
      else
        raise
      end
    end
    
  end

end
