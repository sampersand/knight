# A Number type in Knight.
class Kn::Number < Kn::Value
	# Parse a number from the given stream
	def self.parse(stream) = stream.slice!(/\A\d+/)&.then { new _1.to_i }

	# A `Number` accepts a single argument, which (should) be an Integer.
	def initialize(data) = @data = data

	# Converting a number to an integer is nothing special.
	def to_i = @data

	# Convert 
	def to_s = @data.to_s
	def truthy? = !@data.zero?
	def inspect = "#{self.class}(#@data)"

	def ==(rhs) = self.class == rhs.class && @data == rhs.to_i
	def <=>(rhs) = @data <=> rhs.to_i

	def +(rhs) = Kn::Integer.new(@data + rhs.to_i)
	def -(rhs) = Kn::Integer.new(@data - rhs.to_i)
	def *(rhs) = Kn::Integer.new(@data * rhs.to_i)
	def /(rhs)
		Kn::Integer.new(@data / rhs.to_i)
	rescue ZeroDivisionError
		foo
		raise Kn::RunError, 'cannot divide by zero!', []
	end

	def %(rhs)
		Kn::Integer.new(@data % rhs.to_i)
	rescue ZeroDivisionError
		err = true # for no "backtrace"
	ensure
		raise Kn::RunError, 'cannot modulo by zero!' if err
	end

	def **(rhs) = Kn::Integer.new(@data ** rhs.to_i)
end
