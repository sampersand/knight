#!/usr/bin/env ruby
require_relative 'value'

expr = 
	case $*[0]
	when '-e' then $*[1].dup
	when '-f' then open($*[1], &:read)
	else abort "usage: #$0 [-e 'program'] [-f file]"
	end

Kn::Value.parse(expr).run
