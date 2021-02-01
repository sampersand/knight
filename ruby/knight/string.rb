module Kn
	class String < Value
		# A string starts with either a `'` or `"` and is closed by the starting quote.
		#
		# There are no escape sequences: all characters (but the starting quote) are taken literally.
		# An unterminated quote will raise a ParseError
		def self.parse(stream)
			return unless stream.start_with? '"', "'"

			stream.slice!(/\A(?:'([^']*)'|"([^"]*)")/) or raise ParseError, "no closing quote found for: #{stream}"

			new $+ # `$+` is the last captured group, ie the innards of the quotes.
		end

		def initialize(data) = @data = data.freeze # strings are immutable.

		# When running, we simply get the value of the string.
		def run = @data

		# A String is truthy when it is non empty.
		def truthy? = !@data.empty?

		# A String is considered equal to something else if they have the same class and data.
		def ==(rhs) = self.class == rhs.class && @data == rhs.to_s

		# Adding a string to anything is simply concatenating it with the other.
		def +(rhs) = Kn::String.new("#@data#{rhs}")

		# Multiplying a string by something duplicates the string that many times.
		def *(rhs) = Kn::String.new(@data * rhs.to_i)

		# Compare strings lexicographically.
		def <=>(rhs) = @data <=> rhs.to_s
	end
end
