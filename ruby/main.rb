#!/usr/bin/env ruby

require_relative 'knight/knight'

# Knight::Identifier['a'] = 3
# p Knight::Identifier.new('a').run

=begin
require_relative 'knight'

expr = 
	case $*.length == 2 && $*[0]
	when '-e' then $*[1].dup
	when '-f' then open($*[1], &:read)
	else abort "usage: #$0 [-e 'program'] [-f file]"
	end

Kn::Value.parse(expr).run
=end

