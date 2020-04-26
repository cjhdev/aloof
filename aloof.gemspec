require File.expand_path("../lib/aloof/version", __FILE__)
require 'time'

Gem::Specification.new do |s|
  s.name    = "aloof"
  s.version = Aloof::VERSION
  s.date = Date.today.to_s
  s.summary = "an application protocol"
  s.author  = "Cameron Harper"
  s.email = "contact@cjh.id.au"
  s.files = Dir.glob("lib/**/*.rb") + Dir.glob("bin/**/*")
  s.license = 'MIT'
  s.test_files = Dir.glob("test/**/*.rb")
  
  s.add_development_dependency 'rake'
  s.add_development_dependency 'minitest'
  
  s.add_runtime_dependency 'serialport'
  s.add_runtime_dependency 'digest-crc'
  
  s.required_ruby_version = '>= 2.0'
  
end
