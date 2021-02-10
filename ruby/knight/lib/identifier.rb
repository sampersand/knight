# frozen_string_literal: true
require_relative 'value'

module Kn
	# An identifier within Knight
	class Identifier < Value
		# The list of known variables.
		#
		# One of the few valid reasons to use a class variable:
		# - All instances need to access it
		# - It's not meant to be directly accessible by anyone else
		# - We don't have any subclasses.
		#
		# Note that just on the really off chance that someone defines an `@@env` or something
		# in `Object`, we make this class variable name very specific.
		@@kn_identifier_env = {}

		# Parse an identifier out of the steram.
		#
		# Identifiers are defined as starting with `a-z` or `_`, and then any
		# amount of `a-z`, `_`, or digits.
		def self.parse(stream)
			stream.slice! /\A[a-z_][a-z0-9_]*/ and new($&)
		end

		# Initializes the identifier.
		def initialize(ident)
			@ident = ident
		end

		# You assign an identifier's value by calling `assign`
		def assign(value)
			@@kn_identifier_env[@ident] = value
		end

		# Executing an identifier fetches it from the list of identifiers.
		def run
			@@kn_identifier_env[@ident] or raise RunError, "unknown variable '#@ident'."
		end

		def to_i = run.to_i
		def to_s = run.to_s
		def truthy? = run.truthy?
	end
end
