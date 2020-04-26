require 'minitest/autorun'

require 'aloof'

describe "InputCodec" do

  let(:codec){Aloof::InputCodec}

  describe "get_uint" do
    
    it "gets 0x7f" do
      assert_equal 0x7f, codec.new("\x7f").get_uint
    end
    
    it "gets 0x80" do
      assert_equal 0x80, codec.new("\x81\x80").get_uint
    end
    
    it "gets 0x100" do
      assert_equal 0x100, codec.new("\x82\x01\x00").get_uint
    end
    
    it "gets 0x10000" do
      assert_equal 0x10000, codec.new("\x83\x01\x00\x00").get_uint
    end
    
    it "gets max" do
      assert_equal 0xffffffff, codec.new("\x84\xff\xff\xff\xff").get_uint
    end
  
  end

  describe "get_int" do
    
    it "gets -1" do
      assert_equal -1, codec.new("\x7f").get_int
    end

    it "gets -128" do
      assert_equal -128, codec.new("\x81\x80").get_int
    end

    it "gets 63" do
      assert_equal 63, codec.new("\x3f").get_int
    end

  end
  
end
