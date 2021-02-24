# frozen_string_literal: true
require_relative 'value'

# The Null type in Knight.
module Kn
	class Null < Value
		# Parse a number from the given stream
		def self.parse(stream)
			stream.slice! /\AN[A-Z]*/ and new
		end

		# The sole instance of `Null`.
		INSTANCE = new.freeze # note that we call `new` before we redefine it later.

		# Creates a new `Null`.
		#
		# This technically doesn't create a new instance, but rather returns `INSTANCE`.
		def self.new = INSTANCE

		# Null is always zero.
		def to_i = 0

		# Null's string value is `"null"`
		def to_s = "null"

		# Null is always false.
		def truthy? = false

		# Null is only equal to `INSTANCE`.
		alias == equal?

		# Comparisons with null are disallowed
		def <=>(*)
			raise RunError, 'comparisons with null are disallowed.'
		end
	end
end
