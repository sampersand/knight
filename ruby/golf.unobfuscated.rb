#!/usr/bin/env ruby
class Object
	alias call itself
	def coerce(r) = to_i.coerce(r)

	def to_s = call.to_s
	def to_i = to_s.to_i
	def to_int = to_i
	def to_str = to_s
	def to_sym = to_s.to_sym

	def R(*) = rand(0..0xffff_ffff)
	def P(*) = gets
	def E =  parse!(+to_s).()
	alias B itself
	def ! = call.!
	def C = call.()
	def ` = Kernel.`(to_s)  # `
	def Q = exit(to_i)
	def L = to_s.length
	def D = print(call.inspect)
	def O; if (b="#{a=call}")[-1] == '\\' then print b[..-2] else puts b end; a end
	def +(r) = call + r.()
	def -(r) = call - r.()
	def *(r) = call * r.()
	def /(r) = call / r.()
	def %(r) = call % r.()
	def ^(r) = (call ** r.()).to_i
	def <(r) = call < r.()
	def >(r) = call > r.()
	define_method ?? do s=call;(r=_1.()).is_a?(s.class)&&s==r end
	define_method ?; do call; _1.() end
	define_method ?= do to_sym.send :'=', _1 end
	def &(r) = !(x=call) ? x : r.()
	def |(r) = !(x=call) ? r.(): x
	def W(r) 
		tmp = r.() until !self
		tmp
	end

	def I(t,f) = (!self ? f : t).()
	def G(s, l) = to_s[s.to_i, l.to_i]
	def S(s, l, r)
		t = to_s.dup
		t[s.to_i, l.to_i] = r.to_s
		t
	end
end

class Proc
	def inspect = 'Function(...)'
end

class String
	alias ! empty?
	alias _s *
	def *(r) = _s(r.to_i)
	def inspect = "String(#{self})"
end

class TrueClass
	def ! = false
	def to_i = 1
	def inspect = "Boolean(#{self})"
	def <(_) = false
	def >(r) = !r
end

class FalseClass
	def ! = true
	def inspect = "Boolean(#{self})"
	def <(r) = !!r
	def >(_) = false
end

class Integer
	alias ! zero?
	def inspect = "Number(#{self})"
	def /(rhs) = (to_f / rhs.to_i).truncate
end

class NilClass
	def ! = true
	def to_s = 'null'
	def inspect = 'Null()'
end

class Symbol
	@@ENV={}
	def to_s = call.to_s
	def call = @@ENV.fetch(self)
	def inspect = "Identifier(#{inspect[1..]})"
	define_method ?= do @@ENV[self] = _1.() end
end

def parse!(input)
	input.slice! /\A(?:[\s(){}\[\]:]+|\#[^\n]*)/

	case
	when input.slice!(/\A\d+/) then $&.to_i
	when input.slice!(/\A([TF])[A-Z]*/) then $1 == ?T
	when input.slice!(/\AN[A-Z]*/) then # nil
	when input.slice!(/\A[a-z_][a-z0-9_]*/) then $&.to_sym
	when input.slice!(/\A(?:'([^']*)'|"([^"]*)")/) then $+
	when (meth = Object.instance_method(input[0]) rescue false) then
		input.slice! /\A([A-Z][A-Z_]*|.)/
		args = meth.arity.succ.times.map { parse! input }
		args=[0] if args.empty?
		proc { meth.bind_call(*args) }
	else abort "nothing found"
	end
end

%w%-e -f%.include?(flag=$*.shift)&&(val=$*.shift)&&$*.empty?or abort"usage: #$0 (-e 'program' | -f file)"

parse!(flag=='-e'? +val : open(val, &:read)).()
