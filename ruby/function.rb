module Kn
	class Function < Proc
		@functions = {}

		def self.define(tt=nil, name, &block)
			self[tt || name[0]] = new(name, &block)
		end

		def self.[](name)
			@functions[name]
		end

		def self.[]=(name, func)
			@functions[name] = func
		end

		attr_reader :name

		def initialize(name, ...)
			super(...)
			@name = name
		end

		def inspect = "#{self.class}::#@name"
		alias to_s name

		def call(*) = Value.new(super)

		alias run call

		define 'TRUE' do 
			true
		end

		define 'FALSE' do
			false
		end

		define 'NULL' do
			nil
		end

		define 'PROMPT' do
			$stdin.gets # so we don't read from ARGV
		end

		define 'RANDOM' do 
			rand 0..0xff_ff_ff_ff
		end

		define 'EVAL' do |what|
			Value.new(what.to_s).run
		end

		define 'BLOCK' do |block|
			block
		end

		define 'CALL' do |block|
			block.run.run
		end

		define '`', 'SYSTEM' do |cmd|
			`#{cmd}`
		end

		define 'QUIT' do |code|
			exit code.to_i
		end

		define '!', 'NOT' do |arg|
			!arg
		end

		define 'LENGTH' do |arg|
			arg.to_s.length
		end

		define 'OUTPUT' do |msg|
			text = (msg = msg.run).to_s

			if text.end_with? '\\'
				print text[..-2]
			else
				puts text
			end

			msg
		end

		define '+', 'ADD' do |lhs, rhs|
			lhs + rhs
		end

		define '-', 'SUB' do |lhs, rhs|
			lhs - rhs
		end

		define '*', 'MUL' do |lhs, rhs|
			lhs * rhs
		end

		define '/', 'DIV' do |lhs, rhs|
			lhs / rhs
		end

		define '%', 'MOD' do |lhs, rhs|
			lhs % rhs
		end

		define '^', 'POW' do |lhs, rhs|
			lhs ** rhs
		end

		define '<', 'LTH' do |lhs, rhs|
			lhs < rhs
		end

		define '>', 'GTH' do |lhs, rhs|
			lhs > rhs
		end

		define '?', 'EQL' do |lhs, rhs|
			lhs == rhs
		end

		define '&', 'AND' do |lhs, rhs|
			(lhs = lhs.run).to_b ? rhs.run : lhs
		end

		define '|', 'OR' do |lhs, rhs|
			(lhs = lhs.run).to_b ? lhs : rhs.run
		end

		define ';', 'THEN' do |lhs, rhs|
			lhs.run
			rhs.run
		end

		define 'WHILE' do |cond, body|
			ret = body.run while cond.to_b
			ret
		end

		define '=', 'ASSIGN' do |var, arg|
			Kn::ENVIRONMENT[var.is_a?(Identifier) ? var.data : var.to_s] = arg.run
		end

		define 'IF' do |cond, ift, iff|
			(cond.to_b ? ift : iff).run
		end

		define 'GET' do |string, start, count|
			string.to_s[start.to_i, count.to_i]
		end

		define 'SET' do |string, start, count, repl|
			ret = string.to_s.dup
			ret[start.to_i, count.to_i] = repl.to_s
			ret
		end
	end
end
