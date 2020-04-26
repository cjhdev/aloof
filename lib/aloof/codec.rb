module Aloof

  module Codec
  
    U8  = "C".freeze
    U16 = "S>".freeze
    U32 = "L>".freeze
    U64 = "Q>".freeze
    
    I8  = "c".freeze
    I16 = "s>".freeze
    I32 = "l>".freeze
    I64 = "q>".freeze
    
    FLOAT = "g"
    DOUBLE = "G"

    def sizeof_int(value)
      if value < 0
        if value >= -64
          return 1
        elsif value >= -8192
          return 2
        elsif value >= -32768
          return 3
        elsif value >= -8388608
          return 4
        else
          return 5
        end
      else
        if value <= 0x3f
          return 1            
        elsif value <= 0x1fff
          return 2
        elsif value <= 0x7fff
          return 3
        elsif value <= 0x7fffff
          return 4
        else
          return 5
        end           
      end
    end
    
    def sizeof_uint(value)      
      if value < 0x80
        return 1
      elsif value < 0x100
        return 2
      elsif value < 0x10000
        return 3
      elsif value < 0x1000000
        return 4
      else
        return 5
      end
    end
  
  end

  class OutputCodec
    
    include Codec
    
    attr_reader :buffer
    
    def initialize(buffer="", **opts)
      raise TypeError unless buffer.kind_of? String
      @buffer = buffer      
    end
    
    def put_nil(value=nil)
      self
    end
    
    def put_uint(value)
      case size = sizeof_uint(value) 
      when 1
        @buffer.concat [value].pack(U8)
      else
        @buffer.concat [0x80|(size-1)].pack(U8)
        @buffer.concat [value].pack("L<").slice!(0, size-1).reverse
      end
      self
    end
    
    def put_int(value)
      case size = sizeof_int(value) 
      when 1
        @buffer.concat [value].pack(I8)
      else
        @buffer.concat [0x80|(size-1)].pack(U8)
        @buffer.concat [value].pack(I32).slice!(0, size-1).reverse
      end
      self
    end
    
    def put_u8(value)
      @buffer.concat [value].pack(U8)
      self
    end
  
    def put_u16(value)
      @buffer.concat [value].pack(U16)
      self
    end
    
    def put_u32(value)
      @buffer.concat [value].pack(U32)
      self
    end
    
    def put_u64(value)
      @buffer.concat [value].pack(U64)
      self
    end
    
    def put_i8(value)
      @buffer.concat [value].pack(I8)
      self
    end
  
    def put_i16(value)
      @buffer.concat [value].pack(I16)
      self
    end
  
    def put_i32(value)
      @buffer.concat [value].pack(I32)
      self
    end
    
    def put_i64(value)
      @buffer.concat [value].pack(I64)
      self
    end
  
    def put_string(value)
      put_uint(value.size)
      @buffer.concat value
      self
    end
    
    alias_method :put_blob, :put_string
    
    def put_bool(value)
      @buffer.concat [value ? 1 : 0].pack(U8)
      self
    end
    
    def put_float(value)
      @buffer.concat [value].pack(FLOAT)
      self
    end
    
    def put_double(value)
      @buffer.concat [value].pack(DOUBLE)
      self
    end
    
  end

  class CodecError < StandardError
  end

  class InputCodec

    include Codec

    attr_reader :buffer

    def initialize(buffer)
      @buffer = buffer
      @s = buffer.dup
    end

    def get_nil
      nil
    end

    def get_u8
      raise EOFError if @s.empty?
      @s.slice!(0).unpack(U8).first
    end
  
    def get_u16
      raise EOFError if @s.size < 2
      @s.slice!(0,2).unpack(U16).first
    end
  
    def get_u32
      raise EOFError if @s.size < 4
      @s.slice!(0,4).unpack(U32).first
    end
  
    def get_u64
      raise EOFError if @s.size < 8
      @s.slice!(0,8).unpack(U64).first
    end

    def get_i8
      raise EOFError if @s.empty?
      @s.slice!(0).unpack(I8).first
    end
  
    def get_i16
      raise EOFError if @s.size < 2
       @s.slice!(0,2).unpack(I16).first
    end
  
    def get_i32
      raise EOFError if @s.size < 4
      @s.slice!(0,4).unpack(I32).first
    end
  
    def get_i64
      raise EOFError if @s.size < 8
      @s.slice!(0,8).unpack(I64).first
    end
  
    def get_string
      n = get_uint
      raise EOFError if @s.size < n
      @s.slice!(0,n)
    end
    
    alias_method :get_blob, :get_string
    
    def get_bool
      case get_u8
      when 0
        false
      when 1
        true
      else
        raise CodecError.new "decoded value is not a valid boolean"
      end
    end

    def get_float
      raise EOFError if @s.size < 4
      @s.slice!(0,4).unpack(FLOAT).first
    end
    
    def get_double
      raise EOFError if @s.size < 8
      @s.slice!(0,8).unpack(DOUBLE).first
    end
    
    PADDING1 = "\x00\x00\x00\x00"
    PADDING2 = "\xff\xff\xff\xff"
    
    def get_uint
      first = get_u8
      if first > 0x80
        n = first & 0x7f
        raise EOFError unless @s.size >= n
        raise EncodingError if n > 4
        (@s.slice!(0,n).reverse << PADDING1).unpack("L<").first
      elsif first == 0x80
        raise EncodingError
      else
        first
      end
    end
    
    def get_int
      first = get_u8
      if first > 0x80
        n = first & 0x7f
        raise EOFError unless @s.size >= n
        raise EncodingError if n > 4
        tmp = @s.slice!(0,n).reverse
        if tmp.bytes.last < 0x80
          (tmp << PADDING1).unpack("l<").first
        else
          (tmp << PADDING2).unpack("l<").first
        end
      elsif first == 0x80
        raise EncodingError
      elsif first >= 0x40
        [first|0x80].pack(U8).unpack(I8).first
      else
        first
      end
    end
    
    def eof?
      @s.size == 0
    end
    
  end

end
