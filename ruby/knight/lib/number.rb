# frozen_string_literal: true
require_relative 'value'

# The number type in Knight.
#
# Note that the only numbers within Knight are integers---there are no floats.
module Kn
	class Number < Value
		# Parse a number from the given stream
		def self.parse(stream)
			stream.slice! /\A\d+/ or return

			new $&.to_i
		end

		# Only other numbers can read data. Used for `==`.
		protected attr_reader :data

		# Creates a new `Number` from the given data, which should be a Integer.
		def initialize(data)
			@data = data
		end

		# Converting to an integer is effectively a no op.
		def to_i = @data

		# Converting a number to a string works how you'd expect
		def to_s = @data.to_s

		# Numbers are truthy when they are nonzero.
		def truthy? = @data.nonzero?

		# A number is equal only to another `Number` with the same data.
		def ==(rhs) = rhs.is_a?(self.class) && @data == rhs.data

		# Compares `self` to the integral value of the rhs.
		def <=>(rhs) = @data <=> rhs.to_i

		# Creates a new `Number` with the result of adding `rhs.to_i` to `self`.
		def +(rhs)
			Number.new @data + rhs.to_i
		end

		# Creates a new `Number` with the result of subtracting `rhs.to_i` from `self`.
		def -(rhs)
			Number.new @data - rhs.to_i
		end

		# Creates a new `Number` with the result of multiplying `self` and `rhs.to_i`
		def *(rhs)
			Number.new @data * rhs.to_i
		end

		# Creates a new `Number` with the result of dividing `self` by `rhs.to_i`.
		#
		# If `rhs.to_i` evaluates to zero, a `RunError` is raised.
		def /(rhs)
			# Optimize for the case where `rhs` isn't zero by not checking first.
			Number.new @data / rhs.to_i
		rescue ZeroDivisionError
			# We have to do this dumb little hack to get rid of the "caused by" part of the error.
			# We don't want to leak the fact that `ZeroDivisionError` caused it.
			err = true
		ensure
			raise RunError, 'cannot divide by zero!' if err
		end

		# Creates a new `Number` with the result of moduloing `self` by `rhs.to_i`.
		#
		# If `rhs.to_i` evaluates to zero, a `RunError` is raised.
		def %(rhs)
			# Optimize for the case where `rhs` isn't zero by not checking first.
			Number.new @data % rhs.to_i
		rescue ZeroDivisionError
			# We have to do this dumb little hack to get rid of the "caused by" part of the error.
			# We don't want to leak the fact that `ZeroDivisionError` caused it.
			err = true
		ensure
			raise RunError, 'cannot modulo by zero!' if err
		end

		# Creates a new `Number` with the result of raising `self` to the power of `rhs.to_i`.
		def **(rhs)
			Number.new @data ** rhs.to_i
		end
	end
end
