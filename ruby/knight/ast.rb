module Kn
	class Ast
	end
end
__END__
module Kn
	class Ast
		def self.parse(stream)

		end
	end
end



p Knight.debug
trace_var :$DEBUG do p 1 end
trace_var :$DEBUG do p 2 end

	$DEBUG = 3

untrace_var :$DEBUG
	$DEBUG = 4

