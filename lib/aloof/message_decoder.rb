module Aloof

  class MessageDecoder
  
    def initialize(**opts)
      @logger = opts[:logger]
    end
  
    def decode(input)
      buf = InputCodec.new(input)    
      
      begin
        Message::Base.decode(buf)
      rescue => e        
        puts e
        puts e.backtrace
        nil
      end
      
    end
  
  end

end
