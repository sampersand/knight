#!/usr/bin/env ruby
require_relative 'boolean'
require_relative 'identifier'
require_relative 'number'
require_relative 'null'
require_relative 'string'
require_relative 'function'

class Object
	alias run itself
end

module Kn
	module_function

	def run(stream)
		parse(stream.to_s.dup).run
	end

	def parse(stream)
		# strip leading comments and whitespace
		stream.slice! /\A(?:[\s(){}\[\]:]+|\#[^\n]*)+/m

		# parse out the value
		case
		when stream.slice!(/\A[a-z_][a-z0-9_]*/)         then Identifier.new $&
		when stream.slice!(/\A\d+/)                      then Number.new $&.to_i
		when stream.slice!(/\A([TF])[A-Z_]*/)            then Boolean.new $1 == 'T'
		when stream.slice!(/\AN[A-Z_]*/)                 then Null.new
		when stream.slice!(/\A(?:'([^']*)'|"([^"]*)")/m) then String.new $+
		when (func = Function[name = stream[0]])
			stream.slice! /\A([A-Z_]+|.)/
			Function.new func, name, func.arity.times.map { parse stream }
		else
			raise ParseError, "unknown token start '#{stream[0]}'"
		end
	end
end


if __FILE__ == $0
	def usage = abort("usage: #{File.basename $0} (-e 'program' | -f file)")

	case ($*.length == 2 && $*.shift)
	when '-e' then Kn.run $*.shift
	when '-f' then Kn.run open($*.shift, &:read)
	else usage
	end
end