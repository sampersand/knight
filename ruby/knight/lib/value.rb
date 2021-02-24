# frozen_string_literal: true

module Kn
	class Value
		include Comparable

		# The list of value types.
		@types = []

		# Whenever a new value is created, we automatically add it to the list of types.
		def self.inherited(cls) = @types.push(cls)

		# Try to parse the stream.
		#
		# This goes through all the subclasses of `Value`, calls `.parse` on each one, and
		# returns the first value that matches.
		#
		# If none of the values match, a `ParseError` is raised.
		def self.parse(stream)
			stream.slice! /^(?:[(){}\[\]:\s]+|\#[^\n]*)*/

			@types.each do |type|
				result = type.parse(stream) and return result
			end

			raise ParseError, "unknown token start '#{stream[0]}'"
		end

		# By default, running a value does nothing other than returning itself.
		alias run itself
	end
end
