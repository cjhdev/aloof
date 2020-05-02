module Aloof

  class EncodingError < StandardError
  end
  
  class TimeoutError < StandardError
  end
  
  class AccessError < StandardError
  end
  
  class UnknownOID < AccessError
  end
  
  class AccessDenied < AccessError
  end
  
  class AuthenticationRequired < AccessError
  end
  
  class ArgumentError < AccessError
  end
  
  class TemporaryFailure < AccessError
  end
  
  class ApplicationError < AccessError
  end

end
