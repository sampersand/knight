# frozen_string_literal: true

module Kn
	VERSION = '1.0'

	class Error < StandardError; end
	class ParseError < Error; end
	class RunError < Error; end

	module_function

	def run(value)
		Value.parse(value.to_s.dup).run
	end

	def run!(value)
		exit catch(:exit){ return run(value) }
	rescue => err
		abort err.to_s
	end
end

require_relative 'value'
require_relative 'text'
require_relative 'identifier'
require_relative 'number'
require_relative 'null'
require_relative 'boolean'
require_relative 'function'
