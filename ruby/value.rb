module Kn
	class Value
		include Enumerable

		def self.new(data, ...)
			return super unless self == Value

			case data
			when ::String then String.new(data, ...)
			when Numeric then Number.new(data, ...)
			when true, false then Boolean.new(data, ...)
			when nil then Null.new(...)
			else raise ArgumentError, "unknown data '#{data.inspect}."
			end
		end

		def initialize(value) = @value = value
		def inspect = "#{self.class}(#{@value.inspect})"

		def run = @value

		def to_s = @value.to_s
		def to_i = @value.to_i

		%i(+ - * / % ^ <=>).each do 
		def +(*) = abort "method '#{__method__}' not defined for #{inspect}"
		def -(*) = abort "method '#{__method__}' not defined for #{inspect}"
		def *(*) = abort "method '#{__method__}' not defined for #{inspect}"
		def /(*) = abort "method '#{__method__}' not defined for #{inspect}"
		def %(*) = abort "method '#{__method__}' not defined for #{inspect}"
		def ^(*) = abort "method '#{__method__}' not defined for #{inspect}"
		def <=>(*) = abort "method '#{__method__}' not defined for #{inspect}"

		def ! = Number.new(!to_b)
		def &(rhs) = to_b ? rhs : self
		def |(rhs) = to_b ? self : rhs
	end

	class String < Value
		def to_b = !@value.empty?

		def +(rhs) = String.new("#{self}#{rhs}")
		def *(rhs) = String.new(to_s * rhs.to_i)
		def <=>(rhs) = Number.new(to_s <=> rhs.to_s)
	end

	class Boolean < Value
		def to_b = @value
		def to_i = @value ? 1 : 0

		def <=>(rhs) = Number.new(to_b <=> rhs.to_b)
	end

	class Null < Value
		def self.new = super(nil)
		def inspect = self.class.to_s

		def to_s = "null"
		def to_i = 0
		def to_b = false
	end

	class Number < Value
		def to_b = !@value.zero?

		%i(+ - * / % ** <=>).each do |func|
			define_method func do |rhs|
				Number.new to_i.public_send(func, rhs.to_i)
			end
		end
	end

	class Ast < Value
		KNOWN_METHODS =  
		def method_missing(...) = @value.run.public_send(...)
		def respond_to_missing?(...) = @value.run.public_send(...)
end

require_relative 'ast' unless defined? Kn::Ast
