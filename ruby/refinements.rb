#!/usr/bin/env ruby
module Knight
	refine Integer do
		alias run itself
		def truthy? = !zero? # we need a boolean, and `nonzero?` doesnt do that.
		def dumpstr = "Number(#{self})"

		%i(+ - * / % < >).each do |func|
			define_method func do super _1.to_i end
		end

		def **(rhs) = super(rhs.to_i).to_i
	end

	refine String do
		alias run itself
		def truthy? = !empty?
		def dumpstr = "String(#{self})"

		def *(rhs) = super(rhs.to_i)
		%i(+ < >).each do |func|
			define_method func do super _1.to_s end
		end
	end

	refine TrueClass do
		alias truthy? itself
		alias run itself
		def to_i = 1
		def <(rhs) = false
		def >(rhs) = !rhs.truthy?
		def dumpstr = 'Boolean(true)'
	end

	refine FalseClass do
		alias truthy? itself
		alias run itself
		def to_i = 0
		def <(rhs) = rhs.truthy?
		def >(rhs) = false
		def dumpstr = 'Boolean(false)'
	end

	refine NilClass do
		alias run itself
		def truthy? = false
		def to_s = "null"
		def to_i = 0
		def dumpstr = 'Null()'
	end

	VARS = {}

	refine Symbol do
		def run = VARS.fetch(self)
		def to_s = run.to_s
		def to_i = run.to_i
		def truthy? = run.truthy?
	end

	refine Proc do
		alias run call
		def to_s = run.to_s
		def to_i = run.to_i
		def truthy? = run.truthy?
	end

	FUNCTIONS = {
		?P => -> { gets },
		?R => -> { rand(0..0xffff_ffff) },
		?E => -> { run _1.to_s },
		?B => -> { _1 },
		?C => -> { _1.run.run },
		?! => -> { !_1.truthy? },
		?` => -> { `#{_1}` },
		?L => -> { _1.to_s.length },
		?Q => -> { exit _1.to_i },
		?D => -> { _1.run.tap { |x| x.dumpstr.display } },
		?O => -> { (x=_1.to_s)[-1] == '\\' ? print(x[..-2]) : print(x,"\n") }, # need print so trailing newlines arent trimmed
		?+ => -> { _1.run + _2.run },
		?- => -> { _1.run - _2.run },
		?* => -> { _1.run * _2.run },
		?/ => -> { _1.run / _2.run },
		?% => -> { _1.run % _2.run },
		?^ => -> { _1.run ** _2.run },
		?< => -> { _1.run < _2.run },
		?> => -> { _1.run > _2.run },
		?? => -> { _1.run == _2.run },
		?& => -> { (x=_1.run).truthy? ? _2.run : x },
		?| => -> { (x=_1.run).truthy? ? x : _2.run },
		?; => -> { _1.run ; _2.run },
		?= => -> { VARS[_1] = _2.run },
		?W => -> { _2.run while _1.truthy? },
		?I => -> { _1.truthy? ? _2.run : _3.run },
		?G => -> { _1.to_s[_2.to_i, _3.to_i] },
		?S => -> { (x=_1.to_s.dup)[_2.to_i, _3.to_i] = _4.to_s; x }
	}.freeze
	
	module_function

	using self
	def run(stream) = parse!(stream.dup).run

	def parse!(stream)
		stream.slice! /\A([\s{}()\[\]:]+|#[^\n]*)+/m

		case stream
		when /\A\d+/ then $&.to_i
		when /\A[a-z_][a-z0-9_]*/ then $&.to_sym
		when /\A([TF])[A-Z_]*/ then $1 == 'T'
		when /\AN[A-Z_]*/ then nil
		when /\A"([^"]*)"|\A'([^']*)'/m then $+
		when /\A([A-Z])[A-Z_]*|\A(.)/
			stream.replace $'
			func = FUNCTIONS[$+] or abort "unknown function name '#$+'"
			args = func.arity.times.map { parse! stream }
			return proc { func.(*args) }
		end.tap { stream.replace $' }
	end
end

return unless $0 == __FILE__

case $*.length == 2 && $*.shift
when '-e' then Knight.run $*.shift
when '-f' then Knight.run File.read $*.shift
else abort "usage: #$0 (-e 'program' | -f file)"
end

