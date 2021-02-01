module Kn
	module Function
		# All currently defined functions.
		@functions = {}

		module_function

		# Get a function by the given name, or `nil` if it doesnt exist.
		def [](name) = @functions[name]

		# Define a new function; they must have a single character name.
		def define(name, &block)
			raise ArgumentError, "name must be only one character long (given #{name.length})" unless name.length == 1

			@functions[name] = block
		end

		# Get a random number from 0-0xffffffff
		define 'R' do
			Kn::Number.new rand 0..0xffff_ffff
		end

		# Get a string from stdin and return it, including a trailing newline.
		define 'P' do
			Kn::String.new $stdin.gets # read from stdin in case ARGV's hijacked
		end

		# Evaluate a string as Knight code.
		define 'E' do |msg|
			Kn.run msg.to_s
		end

		# Returns its argument, unevaluated.
		define 'B' do |block|
			block
		end

		# Evaluates its argument twice, so as to call a block of code.
		define 'C' do |block|
			block.run.run
		end

		# Runs a command through the shell.
		define '`' do |command|
			# If the command doesn't return 0, or the command's invalid, raise an error.
			result = `#{command}` rescue nil

			raise RunError, "couldn't run command" if result.nil? || !$?.success?

			Kn::String.new result
		end

		# Quits with the given exit code.
		define 'Q' do |code|
			exit code.to_i
		end

		# Returns the logical negation of the operand.
		define '!' do |arg|
			Kn::Boolean.new !arg.truthy?
		end

		# Converts the argument to a string, then finds its length.
		define 'L' do |string|
			Kn::Number.new string.to_s.length
		end

		# Prints the message out, returning the evaluated message (before conversion to a string.)
		#
		# If it ends in a `\`, the backslash will be removed. Otherwise, a newline will be added to the end.
		define 'O' do |message|
			text = (result = message.run).to_s

			if text.end_with?('\\')
				print text[..-2]
			else
				puts text
			end

			result
		end

		# Adds two values together
		define '+' do |lhs, rhs|
			lhs + rhs
		end

		# Subtracts the second value from the first
		define '-' do |lhs, rhs|
			lhs - rhs
		end

		define '*' do |lhs, rhs|
			lhs * rhs
		end

		define '/' do |lhs, rhs|
			lhs / rhs
		end

		define '%' do |lhs, rhs|
			lhs % rhs
		end

		define '^' do |lhs, rhs|
			lhs ** rhs
		end

		define '<' do |lhs, rhs|
			Boolean.new lhs < rhs
		end

		define '>' do |lhs, rhs|
			Boolean.new lhs > rhs
		end

		define '?' do |lhs, rhs|
			Boolean.new lhs == rhs
		end

		define '&' do |lhs, rhs|
			(lhs = lhs.run).truthy? ? rhs.run : lhs
		end

		define '|' do |lhs, rhs|
			(lhs = lhs.run).truthy? ? lhs : rhs.run
		end

		define ';' do |lhs, rhs|
			lhs.run
			rhs.run
		end

		define 'W' do |cond, body|
			ret = body.run while cond.truthy?
			ret || Null.new
		end

		define '=' do |var, arg|
			Identifier[var.is_a?(Identifier) ? var.data : var.to_s] = arg.run
		end

		define 'I' do |cond, ift, iff|
			(cond.truthy? ? ift : iff).run
		end

		define 'G' do |string, start, count|
			string.to_s[start.to_i, count.to_i]
		end

		define 'S' do |string, start, count, repl|
			ret = string.to_s.dup
			ret[start.to_i, count.to_i] = repl.to_s
			ret
		end
	end
end
