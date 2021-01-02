require_relative 'function'
module Kn
	ENVIRONMENT = {}

	class Value
		include Comparable
		
		attr_reader :data

		def self.parse(stream)
			nil until stream.slice!(/\A(?:#.*?(\n|\z)|\A[\s()\[\]{}:])*/).empty?

			[Number, Text, Boolean, Identifier, Null, Ast]
				.lazy
				.filter_map { _1.parse(stream) }
				.first or abort "unknown character start #{stream[0].inspect}"
		end

		def self.new(value, ...)
			return super unless self == Value

			case value
			when Value       then value
			when String      then Text.new value
			when Numeric     then Number.new value.to_i
			when true, false then Boolean.new value
			when nil         then Null.new value
			else             raise ArgumentError, "no conversion found for #{value.class} to Value."
			end
		end

		def initialize(data) = @data = data
		def inspect = "#{self.class}(#{@data.inspect})"

		alias run itself

		def to_s = run.data.to_s
		def to_i = run.data.to_i
		def to_b = run.to_b

		def ! = !to_b

		def +(rhs) = to_i + rhs.to_i
		def -(rhs) = to_i - rhs.to_i
		def *(rhs) = to_i * rhs.to_i
		def /(rhs) = to_i / rhs.to_i
		def %(rhs) = to_i % rhs.to_i
		def **(rhs) = to_i ** rhs.to_i
		def <=>(rhs) = to_i <=> rhs.to_i

		def hash = @data.hash
		def ==(rhs) = rhs.is_a?(self.class) && @data == rhs.data
		alias eql? ==
	end

	class Number < Value
		def self.parse(stream)
			stream.slice!(/\A\d+/) and new $&.to_i
		end

		def to_b = !@data.zero?
	end

	class Text < Value
		def self.parse(stream)
			case
			when !stream.slice!(/\A(["'])(.*?)(\1|\z)/) then nil
			when $3 then new $2
			else raise ArgumentError, "unterminated string encountered"
			end
		end

		def to_b = !@data.empty?

		def +(rhs) = "#{self}#{rhs}"
		def *(rhs) = @data * rhs.to_i
		def <=>(rhs) = @data <=> rhs.to_s
	end

	class Boolean < Value
		def self.parse(stream)
			stream.slice!(/\A([TF])[A-Z]*/) and new $&.to_i
		end

		alias to_b data
		def to_i = @data ? 1 : 0
	end

	class Null < Value
		def self.parse(stream)
			stream.slice!(/\AN[A-Z]*/) and new
		end

		def initialize = super(nil)

		def to_s = "null"
		def to_i = 0
		def to_b = false
	end

	class Identifier < Value
		def self.parse(stream)
			stream.slice!(/\A[a-z_][a-z0-9_]*/) and new $&
		end

		def run = Kn::ENVIRONMENT.fetch(@data){ raise KeyError, "unknown identifier '#@data'" }
	end

	class Ast < Value
		def self.parse(stream)
			func = Kn::Function[stream[0]] or return
			stream.slice! /\A(?:[A-Z]+|.)/
			new func, func.arity.times.map { Value.parse stream }
		end

		def initialize(func, args)
			@func, @args = func, args
		end

		def inspect = "#{self.class}(#@func, #{@args.map(&:inspect).join(', ')})"
		def run = @func.(*@args)
	end
end
