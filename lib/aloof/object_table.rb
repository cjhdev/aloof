require 'json'
require 'ostruct'

module Aloof

  class ObjectTable
    
    def self.from_json(json)
      
      raw = JSON.parse(json, symbolize_names: true)
      
      raise unless raw.kind_of? Hash
      
      raw = raw.map do |k,v|
        
        raise unless v.kind_of? Hash
        
        value = OpenStruct.new
        
        value.name = k.to_sym
        
        case v[:oid]
        when String
          value.oid = v[:oid].to_i(16)
        when Integer
          value.oid = v[:oid].to_i
        else
          raise
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
      
      name = name.to_sym
      oid = opts[:oid]||(@defs.map{|k,v|v.oid}.sort.last + 1)
      
      @defs[name] = OpenStruct.new(oid: oid, name: name, type: opts[:type])
      self
    end
    
    def lookup_name(name)
      @defs[name.to_sym] if name
    end
    
    def to_json
      @defs.to_json
    end
  
  end

end
