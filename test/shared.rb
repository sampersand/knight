module Kn; end

Identifier = Struct.new :ident
Function = Struct.new :ast

module Kn::Tests

	def execute(expr, chomp=true)
		IO.pipe do |r, w|
			system(@program_path, '-e', expr, out: w, err: :close) or throw :call_failed

			w.close
			r.read.tap { |x| x.chomp! if chomp }
		end
	end

	def assert_fails
		assert_throws(:call_failed) { yield }
	end

	def assert_runs
		yield
	end

	def dump(expr, *rest)
		execute("D #{expr}", *rest)
	end

	def eval(expr)
		parse dump expr
	end

	def to_string(expr)
		val = eval "+ '' #{expr}"
		raise "not a string: #{val.inspect}" unless val.is_a? String
		val
	end

	def to_number(expr)
		val = eval "+ 0 #{expr}"
		raise "not a number: #{val.inspect}" unless val.is_a? Integer
		val
	end

	def to_boolean(expr)
		val = eval "I #{expr} TRUE FALSE"
		raise "not a boolean: #{val.inspect}" unless val == true || val == false
		val
	end
end
