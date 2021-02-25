# frozen_string_literal: true
require_relative 'value'

# The string type within Knight.
module Kn
	class Text < Value
		# A string starts with either a `'` or `"` and is closed by the starting quote.
		#
		# There are no escape sequences: all characters (but the starting quote) are taken literally.
		# An unterminated quote will raise a ParseError
		def self.parse(stream)
			return unless stream.start_with? '"', "'"

			stream.slice! /\A(?:'([^']*)'|"([^"]*)")/ or raise ParseError, "no closing quote found for: #{stream}"

			new $+ # `$+` is the last captured group, ie the innards of the quotes.
		end

		# Only other `Text`s can read data. Used for `==`.
		protected attr_reader :data

		# Creates a new `Text` from the given data, which should be a String.
		def initialize(data)
			@data = data.freeze # Texts are immutable.
			freeze
		end

		# As per the specs, only strings that start with a number need to be convertible.
		#
		# However, we're allowed to define a superset, so we'll also allow things that start with
		# leading whitespace.
		# Converting to a string is simply accessing its data.
		def to_i = @data.to_i

		# Simply returns the string associated with `self`
		def to_s = @data # data is frozen already

		# A Text is truthy when nonempty.
		def truthy? = !@data.empty?

		# A Text is considered equal to something else if they have the same class and data.
		def ==(rhs) = rhs.is_a?(self.class) && @data == rhs.data

		# Compare texts lexicographically.
		def <=>(rhs) = @data <=> rhs.to_s

		# Creates a new `Text` with `rhs.to_s` concatenated on the end.
		def +(rhs)
			Text.new @data + rhs.to_s
		end

		# Creates a new `Text` with `self` duplicated `rhs.to_i` times.
		def *(rhs) 
			Text.new @data * rhs.to_i
		end
	end
end
