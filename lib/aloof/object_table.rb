require 'json'
require 'ostruct'

module Aloof

  class ObjectTable
    
    def self.from_json(json)
      
      raw = JSON.parse(json, symbolize_names: true)
      
      raise unless raw.kind_of? Hash
      
      raw.map do |k,v|
        
        raise unless v.kind_of? Hash
        
        value = OpenStruct.new
        
        if v[:oid].kind_of? String
          value.oid = v[:oid].to_i(16)
        else
          value.oid = v[:oid].to_i
        end
        
        value.type = v[:type].downcase.to_sym
        
        raise unless Message::Base::TYPES.include? value.type
        
        [
          k,
          value
        ]
      
      end.to_h
      
      self.new(raw)
      
    end
    
    def initialize(defs={})    
      @defs = defs
    end
    
    def add(name, **opts)
      @defs[name.to_sym] = OpenStruct.new(**opts)
      self
    end
    
    def lookup_name(name)
      @defs[name.to_sym]
    end
    
    def to_json
      @defs.to_json
    end
  
  end

end
