# frozen_string_literal: true

module Kn
	class Function
		@funcs = {}

		def self.register(name, &block) 
			raise ArgumentError, 'name must be exactly one character' unless name.length == 1
			@funcs[name] = block
		end

		def self.[](name) = @funcs[name]

		def initialize(func, name, args)
			@func = func
			@name = name
			@args = args
		end

		def inspect = "Function(#@name#{@args.map { ", #{_1.inspect}" }.join})"

		def run = @func.call(*@args)

		undef ==
		undef to_s

		def respond_to_missing?(...) = run.respond_to_missing?(...)
		def method_missing(...) = run.public_send(...)

		register 'P' do String.new gets.chomp end
		register 'R' do Number.new rand 0..0xffff_ffff end

		register 'E' do Kn.run _1 end
		register 'B' do _1 end
		register 'C' do _1.run.run end
		register '`' do String.new `#{p(_1.to_s)}` end
		register 'Q' do exit _1.to_i end
		register '!' do Boolean.new !_1.truthy? end
		register 'L' do Number.new _1.to_s.length end
		register 'D' do print _1.run.inspect end
		register 'O' do
			str = _1.to_s

			if str[-1] == '\\'
				print str[0..-2]
			else
				puts str
			end

			Null.new
		end

		register '+' do _1 + _2 end
		register '-' do _1 - _2 end
		register '*' do _1 * _2 end
		register '/' do _1 / _2 end
		register '%' do _1 % _2 end
		register '^' do _1 ** _2 end
		register '?' do Boolean.new _1.run == _2.run end
		register '<' do Boolean.new _1 < _2 end
		register '>' do Boolean.new _1 > _2 end
		register '&' do (l = _1.run).truthy? ? _2.run : l end
		register '|' do (l = _1.run).truthy? ? l : _2.run end
		register ';' do _1.run; _2.run end
		register '=' do _1.value = _2.run end
		register 'W' do _2.run while _1.truthy? ; Null.new end

		register 'I' do _1.truthy? ? _2.run : _3.run end
		register 'G' do String.new _1.to_s[_2.to_i, _3.to_i] end

		register 'S' do
			str = _1.to_s.dup
			str[_2.to_i, _3.to_i] = _4.to_s
			String.new str
		end
	end
end
