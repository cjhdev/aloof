require 'minitest/autorun'

require 'aloof'

describe "MessageDecoder"  do

  describe "WriteCommand" do
  
    tests = [
      
      {invoke: 0, oid: 42, type: :nil, value: nil, bytes: [3,0,0,1,0,42,0].pack("C*")},    
      
      {invoke: 0, oid: 42, type: :u8, value: 35, bytes: [3,0,0,1,0,42,1,35].pack("C*")},    
      {invoke: 0, oid: 42, type: :u16, value: 35, bytes: [3,0,0,1,0,42,2,0,35].pack("C*")},    
      {invoke: 0, oid: 42, type: :u32, value: 35, bytes: [3,0,0,1,0,42,3,0,0,0,35].pack("C*")},    
      {invoke: 0, oid: 42, type: :u64, value: 35, bytes: [3,0,0,1,0,42,4,0,0,0,0,0,0,0,35].pack("C*")},    
      
      {invoke: 0, oid: 42, type: :i8, value: 35, bytes: [3,0,0,1,0,42,5,35].pack("C*")},    
      {invoke: 0, oid: 42, type: :i16, value: 35, bytes: [3,0,0,1,0,42,6,0,35].pack("C*")},    
      {invoke: 0, oid: 42, type: :i32, value: 35, bytes: [3,0,0,1,0,42,7,0,0,0,35].pack("C*")},    
      {invoke: 0, oid: 42, type: :i64, value: 35, bytes: [3,0,0,1,0,42,8,0,0,0,0,0,0,0,35].pack("C*")},    
      
      {invoke: 0, oid: 42, type: :bool, value: true, bytes: [3,0,0,1,0,42,9,1].pack("C*")},    
      
      {invoke: 0, oid: 42, type: :float, value: 42, bytes: [3,0,0,1,0,42,10,0x42,0x28,0,0].pack("C*")},    
      {invoke: 0, oid: 42, type: :double, value: 42, bytes: [3,0,0,1,0,42,11,0x40,0x45,0,0,0,0,0,0].pack("C*")},          
      
      #{invoke: 0, oid: 42, type: :string, value: "hello", bytes: [3,0,0,1,0,42,12,5,'hello'].pack("C7a5")},    
      #{invoke: 0, oid: 42, type: :blob, value: "hello", bytes: [3,0,0,1,0,42,13,5,'hello'].pack("C7a5")},    
    ]
  
    tests.each do |params|
    
      it "encode_oid#{params[:oid]}_type#{params[:type]}" do
      
        output = Aloof::Message::WriteCommand.new(params[:oid], params[:type], params[:value]).encode        
        assert_equal params[:bytes], output
      
      end
      
      it "decode_oid#{params[:oid]}_type#{params[:type]}" do
      
        msg = Aloof::MessageDecoder.new.decode(params[:bytes])
      
        assert_instance_of Aloof::Message::Command, msg
        
        assert_equal params[:invoke], msg.invoke_id
        assert_equal params[:oid], msg.oid
        assert_equal params[:type], msg.type
        
        if params[:value].nil?
        
          assert_nil msg.value
        
        else
        
          assert_equal params[:value], msg.value
          
        end
          
      end
      
    end
    
  end

  describe "WriteResponse" do
  
    tests = [
      {result: :success, reason: nil, bytes: [0,0].pack("C*")},    
    ]
    
    tests.each do |params|
  
      it "encode_oid#{params[:oid]}_type#{params[:type]}" do
      
        output = Aloof::Message::WriteResponse.new(params[:result], params[:reason]).encode        
        assert_equal params[:bytes], output
      
      end
      
      it "decode_oid#{params[:oid]}_type#{params[:type]}" do
      
        msg = Aloof::Message::WriteCommand.decode(Aloof::InputCodec.new(params[:bytes]))
        
        assert_equal params[:result], msg.result
        
        if params[:reason].nil?
        
          assert_nil msg.reason
        
        else
        
          assert_equal params[:reason], msg.reason
          
        end
          
      end
  
    end
  
  end

end
