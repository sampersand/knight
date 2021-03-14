# frozen_string_literal: true

module Kn
	class Boolean
		def inspect = "Boolean(#{to_s})"

		# note that we call `new` before we redefine it later.
		class << (TRUE = new)
			def to_i = 1
			def to_s = 'true'
			def truthy? = true

			def <(rhs) = false
			def >(rhs) = !rhs.truthy?
			freeze
		end

		# note that we call `new` before we redefine it later.
		class << (FALSE = new)
			def to_i = 0
			def to_s = 'false'
			def truthy? = false

			def <(rhs) = rhs.truthy?
			def >(rhs) = false
			freeze
		end

		def self.new(bool) = bool ? TRUE : FALSE
	end
end