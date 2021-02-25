class String
    def to_b; !empty? end
    def coerce(rhs) [method(rhs.class.name).(self), rhs] end
end

class Numeric
		def to_b = !zero?

    # def to_b; !zero? end
    alias to_str to_s
end

class TrueClass
    alias to_b itself
    alias to_str to_s
    def coerce(rhs) 1.coerce(rhs) end
end

class FalseClass
    alias to_b itself
    alias to_str to_s
    def coerce(rhs) 0.coerce(rhs) end
end

class NilClass
    def to_b; false end
    alias to_str inspect
    def coerce(rhs) 1.coerce(rhs) end
end

Ident = Class.new String

def tokenize(input)
	return to_enum(__method__, input) unless block_given?
	until input.empty?
		yield case 
		when input.slice!(/\A[\s()\[\]{}:]+/) then next
		when input.slice!(/\A#.*$/) then next
		when input.slice!(/\A\d+/) then $&.to_i
		when input.slice!(/\A[a-z_]\w*/) then Ident.new $&
		when input.slice!(/\A(['"])((?:\\.|.)*?)\1/) then $2
		when input.slice!(/\A([TFN])[A-Z]*/) then $1 == 'T' || $1 == 'F' ? false : nil
		when input.slice!(/\A(?:[A-Z]+|[-+*\/%^|&!><;=])/) then $&[0].to_sym
		else raise "invalid token start '#{input[0]}'"
		end
	end
end

def parse(input)
	case token = input.next
	when String, Symbol, Integer, true, false, nil then [token]
	when :cO, :cE, :cB, :'c`', :'c!', :'cQ' then [input, parse(input)]
	when :'c-', :'c+', :'c*', :'c/', :'c%', :'c^', :'c&', 
	     :'c|', :'c!', :'c<', :'c>', :'c;', :'c=', :W, :R then [input, parse(input), parse(input)]
	when :cI then [input, parse(input), parse(input), parse(input)]
	else raise "unknown token '$token'."
	end
end
__END__
def run(cmd, arg1=nil, arg2=nil, arg3=nil)
	case cmd
	when Ident then $env[cmd]
	when Number, String, true, false, nil then [input]
	when 'P' then [gets.chomp]
	when 'B' then input[1]
	when 'C' then run run input[1]
	     if ('i' eq substr $cmd, 0, 1) { $env{substr($cmd, 1)};
	} elsif ($cmd =~ /\A([^c]|c[TFN])/) { [$cmd];
	} elsif ($cmd eq 'cP') { ['s' . <STDIN>];
	} elsif ($cmd eq 'cB') { $arg1;
	} elsif ($cmd eq 'cC') { &run(&run($arg1));
	} elsif ($cmd eq 'c`') { ['s' . `@{[to_string &run($arg1)->[0]]}`];
	} elsif ($cmd eq 'cQ') { exit to_num($arg1);
	} elsif ($cmd eq 'c!') { [to_bool($arg1) ? 'cF' : 'cT'];
	} elsif ($cmd eq 'cO') {
		$ret = &run($arg1);
		my $str = to_string($ret, 1);

		if ('\\' eq substr $str, -1) {
			print substr $str, length($str) - 1;
		} else {
			print $str, "\n";
		}
		[$ret];
	} elsif ($cmd eq 'cW') {
		$ret = ['cN'];
		$ret = &run($arg2) while to_bool($arg1);
		[$ret];
	} elsif ($cmd eq 'cR') { ['n' . (rand() * to_num($arg1) + to_run($arg2))];
	} elsif ($cmd eq 'c+') {
		if (($lhs = &run($arg1))->[0] =~ /\As/) {
			['s' . to_string($lhs, 1) . to_string($arg2)];
		} else {
			['n' . (to_num($lhs, 1) + to_num($arg2))];
		}
	} elsif ($cmd eq 'c-') { ['n' . (to_num($arg1) - to_num($arg2))];
	} elsif ($cmd eq 'c*') { ['n' . (to_num($arg1) * to_num($arg2))];
	} elsif ($cmd eq 'c/') { ['n' . (to_num($arg1) / to_num($arg2))];
	} elsif ($cmd eq 'c%') { ['n' . (to_num($arg1) % to_num($arg2))];
	} elsif ($cmd eq 'c^') { ['n' . (to_num($arg1) **to_num($arg2))];
	} elsif ($cmd eq 'c<') {
		if (($lhs = &run($arg1))->[0] =~ /\As/) {
			[(to_string($lhs, 1) lt to_string($arg2)) ? 'cT' : 'cF'];
		} else {
			[to_num($lhs, 1) < to_num($arg2) ? 'cT' : 'cF'];
		}
	} elsif ($cmd eq 'c>') {
		if (($lhs = &run($arg1))->[0] =~ /\As/) {
			[to_string($lhs, 1) gt to_string($arg2) ? 'cT' : 'cF'];
		} else {
			[to_num($lhs, 1) > to_num($arg2) ? 'cT' : 'cF'];
		}
	} elsif ($cmd eq 'c;') { &run($arg1); &run($arg2);
	} elsif ($cmd eq 'c&') { [to_bool($arg1) & to_bool($arg2) ? 'cT' : 'cF'];
	} elsif ($cmd eq 'c|') { [to_bool($arg1) | to_bool($arg2) ? 'cT' : 'cF'] ;
	} elsif ($cmd eq 'c=') { $env{substr($arg1->[0], 1)} = $lhs = &run($arg2); $lhs;
	} elsif ($cmd eq 'cI') { [&run(to_bool($arg1) ? $arg2 : $arg3)];
	} else { die "unknown command '$cmd'.";
	}
}


p tokenize('
; = a 0
; = b 1
; = n 10
; OUTPUT (+ (+ "fib(" n) ")=\")
; WHILE (> n 1)
  ; = tmp b
  ; = b (+ b a)
  ; = a tmp
  = n (- n 1)
OUTPUT b
').to_a
