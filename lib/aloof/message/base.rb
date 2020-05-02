module Aloof::Message

  class Base

    require_relative "hello"
    require_relative "hellohello"
    require_relative "error"
    require_relative "command"
    require_relative "response"
    require_relative "alert"
    
    MESSAGES = [
      Hello,
      HelloHello,
      Error,
      Command,
      Response,
      Alert
    ]
    
    TYPES = [
      :nil,
      
      :u8,
      :u16,
      :u32,
      :u64,
      
      :i8,
      :i16,
      :i32,
      :i64,
      
      :bool,
      
      :float,
      :double,
      
      :string,      
      :blob    
    ]
    
    def self.decode(buf)
    
      msg = MESSAGES[buf.get_uint]
      msg.decode(buf) if msg
      
    end
    
    def self.get_value(buf)
      type_tag = buf.get_u8      
      type = TYPES[type_tag] if type_tag      
      raise EncodingError unless type      
      return type, buf.send("get_#{type}".to_sym)
    end
    
    def self.put_value(buf, type, value)            
      buf.put_u8(TYPES.index(type))
      buf.send("put_#{type}".to_sym, value)
    end
    
    ACCESS_RESULT_REASON = [
      :unkown_oid,
      :access_denied,
      :authentication_required,
      :argument_error,
      :temporary_failure,
      :application_error    
    ]
    
    def self.get_access_result(buf)
      case buf.get_uint
      when 0
        return :success
      when 1
        reason_tag = buf.get_uint
        reason = ACCESS_RESULT_REASON[reason_tag]
        raise EncodingError unless reason
        return :failure, reason        
      else
        raise EncodingError
      end
    end
    
    def self.put_access_result(buf, result, reason)
      case result
      when :success
        buf.put_uint(0)
      when :failure
        raise unless ACCESS_RESULT_REASON.index(reason)
        buf.put_uint(1)
        buf.put_uint(ACCESS_RESULT_REASON.index(reason))
      else
        raise
      end
    end  
  
  end

end
