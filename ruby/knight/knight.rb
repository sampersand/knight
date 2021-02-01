module Kn
	VERSION = '1.0'

	@debug = $DEBUG

	class << self
		attr_accessor :debug
		alias debug? debug
	end

	trace_var :$DEBUG do |val|
		Kn.debug = val
	end

	class Error < StandardError; end
	class ParserError < Error; end
	class RunError < Error; end
end

require_relative 'value'
require_relative 'string'
require_relative 'identifier'
require_relative 'number'
