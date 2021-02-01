# #!/usr/bin/env ruby
# require_relative 'value'

# expr = 
# 	case $*.length == 2 && $*[0]
# 	when '-e' then $*[1].dup
# 	when '-f' then open($*[1], &:read)
# 	else abort "usage: #$0 [-e 'program'] [-f file]"
# 	end

# Kn::Value.parse(expr).run


require 'benchmark'

TESTS = 10_000
Benchmark.bmbm do |bench|
	bench.report "[*..]" do
		TESTS.times { [*0..1000] }
	end

	bench.report ".to_a" do
		TESTS.times { 1000.times.to_a }
	end

end
