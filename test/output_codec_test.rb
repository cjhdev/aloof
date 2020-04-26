require 'minitest/autorun'

require 'aloof'

describe "OutputCodec" do

  let(:codec){Aloof::OutputCodec.new}

  describe "put_uint" do
    
    it "puts 0x7f" do
      assert_equal "\x7f".bytes, codec.put_uint(0x7f).buffer.bytes
    end
    
    it "puts 0x80" do
      assert_equal "\x81\x80".bytes, codec.put_uint(0x80).buffer.bytes
    end
    
    it "puts 0x100" do
      assert_equal "\x82\x01\x00".bytes, codec.put_uint(0x100).buffer.bytes
    end
    
    it "puts 0x10000" do
      assert_equal "\x83\x01\x00\x00".bytes, codec.put_uint(0x10000).buffer.bytes
    end
    
    it "puts max" do
      assert_equal "\x84\xff\xff\xff\xff".bytes, codec.put_uint(2**32-1).buffer.bytes
    end
  
  end
  
  describe "put_string" do
    
    it "puts empty" do
      assert_equal "\x00".bytes, codec.put_string("").buffer.bytes
    end
    
    it "puts hello world" do
      assert_equal "\x0b\x68\x65\x6c\x6c\x6f\x20\x77\x6f\x72\x6c\x64".bytes, codec.put_string("hello world").buffer.bytes
    end
    
  end
  
  describe "put bool" do
  
    it "puts true" do
      assert_equal "\x01".bytes, codec.put_bool(true).buffer.bytes
    end
  
    it "puts false" do
      assert_equal "\x00".bytes, codec.put_bool(false).buffer.bytes
    end
  
  end
  
end
