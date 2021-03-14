# frozen_string_literal: true

module Kn
	class Number
		include Comparable

		def initialize(num) = @num = num

		def inspect = "Number(#@num)"

		def to_i = @num
		def to_s = @num.to_s
		def truthy? = @num.nonzero?

		def +(rhs) = Number.new(@num + rhs.to_i)
		def -(rhs) = Number.new(@num - rhs.to_i)
		def *(rhs) = Number.new(@num * rhs.to_i)

		def /(rhs) 
			Number.new @num.fdiv(rhs.to_i).truncate
		rescue ZeroDivisionError => err
			raise RunError, 'cannot divide by zero'
		end

		def %(rhs) 
			Number.new @num % rhs.to_i
		rescue ZeroDivisionError => err
			raise RunError, 'cannot modulo by zero'
		end

		def **(rhs) = Number.new((@num ** rhs.to_i).to_i)

		def <=>(rhs) = @num <=> rhs.to_i
		def ==(rhs) = (rhs = rhs.run).is_a?(self.class) && @num == rhs.to_i
	end
end
