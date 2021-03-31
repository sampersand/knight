# frozen_string_literal: true
require_relative 'value'

# The Boolean type in Knight.
module Kn
	refine TrueClass do
	class Boolean < Value
		# Parse a number from the given stream
		def self.parse(stream)
			stream.slice! /\A([TF])[A-Z]*/ and new($1 == 'T')
		end

		# The instance of `Boolean` that represents truthiness.
		TRUE = new # note that we call `new` before we redefine it later.
		TRUE.instance_exec do
			def to_i = 1
			def to_s = "true"
			def truthy? = true
			freeze
		end

		# The instance of `Boolean` that represents falsehood.
		FALSE = new
		FALSE.instance_exec do
			def to_i = 0
			def to_s = "false"
			def truthy? = false
			freeze
		end

		# Creates a new `Boolean` from the given data, which should be true or false.
		#
		# This technically doesn't create a new instance, but rather returns `TRUE` or `FALSE`.
		def self.new(data) = data ? TRUE : FALSE

		# Booleans are only equal to other instances of themselves.
		alias == equal?

		# Compares the truthiness of `self` with the truthiness of `rhs`.
		def <=>(rhs)
			to_i <=> Boolean.new(rhs.truthy?).to_i
		end
	end
end
