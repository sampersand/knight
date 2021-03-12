# frozen_string_literal: true

module Kn
	class String
		include Comparable

		def initialize(str) = @str = str

		def inspect = "String(#@str)"

		def to_i = @str.to_i
		def to_s = @str
		def truthy? = !@str.empty?

		def +(rhs) = String.new(@str + rhs.to_s)
		def *(rhs) = String.new(@str * rhs.to_i)

		def <=>(rhs) = @str <=> rhs.to_s
		def ==(rhs) = rhs.is_a?(self.class) && @str == rhs.to_s
	end
end
