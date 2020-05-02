OBJECT_TABLE = Aloof::ObjectTable.new

OBJECT_TABLE.add(:test_nil, oid: 0, type: :nil)
OBJECT_TABLE.add(:test_u8, oid: 1, type: :u8)
OBJECT_TABLE.add(:test_u16, oid: 2, type: :u16)
OBJECT_TABLE.add(:test_u32, oid: 3, type: :u32)
OBJECT_TABLE.add(:test_u64, oid: 4, type: :u64)
OBJECT_TABLE.add(:test_i8, oid: 5, type: :i8)
OBJECT_TABLE.add(:test_i16, oid: 6, type: :i16)
OBJECT_TABLE.add(:test_i32, oid: 7, type: :i32)
OBJECT_TABLE.add(:test_i64, oid: 8, type: :i64)
OBJECT_TABLE.add(:test_bool, oid: 9, type: :bool)
OBJECT_TABLE.add(:test_float, oid: 10, type: :float)
OBJECT_TABLE.add(:test_double, oid: 11, type: :double)
OBJECT_TABLE.add(:test_string, oid: 12, type: :string)
OBJECT_TABLE.add(:test_blob, oid: 13, type: :blob)

OBJECT_TABLE.add(:test_unknown, oid: 14, type: :nil)
OBJECT_TABLE.add(:test_no_access, oid: 15, type: :nil)
OBJECT_TABLE.add(:test_auth, oid: 16, type: :nil)
OBJECT_TABLE.add(:test_arg, oid: 17, type: :nil)
OBJECT_TABLE.add(:test_temp, oid: 18, type: :nil)
OBJECT_TABLE.add(:test_app, oid: 19, type: :nil)

sequence do

  # run all of these tests
  continue_on_failure
  
  setup do
  
    step do
      #locals.pid = spawn "./../bin/test_server"
      locals.client = Aloof::Client.new(object_table: OBJECT_TABLE, transport: :tcp, port: 4567, timeout: 1)
      locals.client.open
    end
  
  end
  
  desc "get nil"
  step do
    assert_nil locals.client.read("test_nil")
  end
  
  desc "get u8"
  step do
    assert_equal 2**7, locals.client.read("test_u8")
  end
  
  desc "get u16"
  step do
    assert_equal 2**8, locals.client.read("test_u16")
  end
  
  desc "get u32"
  step do    
    assert_equal 2**16, locals.client.read("test_u32")
  end
  
  desc "get u64"
  step do    
    assert_equal 2**32, locals.client.read("test_u64")
  end
  
  desc "get i8"
  step do    
    assert_equal -42, locals.client.read("test_i8")
  end
  
  desc "get i16"
  step do    
    assert_equal -42, locals.client.read("test_i16")
  end
  
  desc "get i32"
  step do    
    assert_equal -42, locals.client.read("test_i32")
  end
  
  desc "get i64"
  step do    
    assert_equal -42, locals.client.read("test_i64")
  end
  
  desc "get bool"
  step do    
    assert_equal true, locals.client.read("test_bool")
  end
  
  desc "get float"
  step do    
    assert_equal 42, locals.client.read("test_float")
  end
  
  desc "get double"
  step do    
    assert_equal 42, locals.client.read("test_double")
  end
  
  desc "get string"
  step do    
    assert_equal "hello", locals.client.read("test_string")
  end
  
  desc "get blob"
  step do    
    assert_equal "hello", locals.client.read("test_blob")
  end
  
  desc "get object unknown"
  step do
    assert_raises Aloof::UnknownOID do
      locals.client.read(:test_unknown)
    end
  end
  
  
  step do
    assert_raises Aloof::AccessDenied do
      locals.client.read(:test_no_access)
    end
  end
  
  
  step do
    assert_raises Aloof::AuthenticationRequired do
      locals.client.read(:test_auth)
    end
  end
  
  step do
    assert_raises Aloof::ArgumentError do
      locals.client.read(:test_arg)
    end
  end
  
  step do
    assert_raises Aloof::TemporaryFailure do
      locals.client.read(:test_temp)
    end
  end
  
  step do
    assert_raises Aloof::ApplicationError do
      locals.client.read(:test_app)
    end
  end
  
  teardown do
    
    step do
      locals.client.close
    end
    
  end

end
