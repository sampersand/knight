require 'minitest/autorun'
require_relative 'shared'

module PHP
	include Kn::Tests

	def parse(expr)
		case expr
		when /\Anull\(\)\z/ then :nil
		when /\Astring\((.*?)\)\z/m then $1
		when /\Aboolean\((true|false)\)\z/ then $1 == 'true'
		when /\Anumber\(((?:-(?!0\)))?\d+)\)\z/ then $1.to_i # `-0` is invalid.
		when /\Afunction\((.*?)\)/m then Function.new $1
		when /\Aidentifier\(([_a-z][\w_]*)\)/ then Identifier.new $1
		else fail "bad expression: #{expr.inspect}"
		end
	end

	def self.extended(inst)
		inst.instance_exec do
			@program_path = '../php/bin/knight'
		end
	end
end

$current_instance = PHP

require_relative 'tests/number'
require_relative 'tests/null'
require_relative 'tests/boolean'
require_relative 'tests/string'
require_relative 'tests/block'
