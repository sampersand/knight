class Object
	alias run itself
	def to_str = run.to_s

	def to_i = run ? 1 : 0
	def falsey? = to_i.zero?
	def coerce(rhs) = [to_i, rhs]
end

class Ident < String
	$environment = {}

	def to_i = run.to_i
	def run = $environment.fetch(to_s) { abort "unknown identifier '#{self}" }
	def self.===(stream) = $match = stream.slice!(/\A[a-z_][a-z_0-9]*/)&.then { new _1.freeze }
end

class String
	alias falsey? empty?
	def self.===(stream)
		stream.slice! /\A(?:'([^']*)'|"([^"]*)")/ and return $match = $+.freeze
		abort "unmatched quote: #{stream}" if stream.start_with? '"', "'"
	end
end

class NilClass
	def to_s = 'null'
	def self.===(stream) = stream.slice!(/\AN[A-Z]*/) 
end

def Integer.===(stream) = $match = stream.slice!(/\A\d+/)&.to_i
def FalseClass.===(stream) = stream.slice!(/\AF[A-Z]*/) 
def TrueClass.===(stream) = stream.slice!(/\AT[A-Z]*/) 

Func = Struct.new(:func, :args) do
	def run = func.(*args)

	@funcs = {}
	def self.===(stream)
		func = stream.slice!(/\A([A-Z]+|[-+*\/%^|&!><;=])/) or return
	end

	def self.P = $stdin.gets
	def self.R = rand(0..0xffff_ffff)
	def self.E(a) = parse(a).run
	def self.B(a) = a
	def self.C(a) = a.run.run
	def self.`(a) = `#{a}`
	def self.Q(a) = exit(a)
	def self.!(a) = !a.falsey?
	def self.L(a) = a.to_s.length
	def self.O(a) = ((l=(a=a.run).to_s)[-1] == '\\' ? print(l[..-2]) : puts(l)).then { a }
	def self.+(a, b) = a.run + b.run
	def self.-(a, b) = a.run - b.run
	def self.*(a, b) = a.run * b.run
	def self./(a, b) = a.run / b.run
	def self.%(a, b) = a.run % b.run
	def self.^(a, b) = a.run ** b.run
	def self.<(a, b) = a.run < b.run
	def self.>(a, b) = a.run > b.run
	define_singleton_method :'?' do |a, b| a == b end
	# def self.method_defined(x) p x end
	# 	def a; end
	# 	def self.a; end
	define_method :';' do 1 end
		p new.method(';').call
	@funcs[3]
end

exit

def parse(stream)
	case stream.to_s
	when TrueClass then true
	when FalseClass then false
	when NilClass then
	when Integer, String, Ident, Func then $match
	else abort "unknown stream start '#{stream[0]}'"
	end
end
__END__
p 1.to_str
exit
p Ident.new('a').run

p false.to_i

case (x=%(12abc))
when Integer then p $match
when String then p [$match]
else p "neither"
end
p x
__END__
class TrueClass
	include Value
	def to_i = 1
end

class FalseClass
	include Value
	def to_1 = 0
end

class NilClass
	include Value
	def to_str = "null"
	def to_i = 0
end

class Ident < String
	def run = $environment.fetch(self){ abort "unknown ident #{self}" }
end

def next_token(input)
	input.slice! /\A([\s()\[\]{}:]+|#.*\n)*/

	case input
	when /\A\d+/ then $&.to_i
	when /\A[a-z_]\w*/ then Ident.new $&
	when /\A'([^']*)'/, /\A"([^"]*)"/ then $1
	when /\A['"]/ then abort "unmatched quote: #{input}"
	when /\A[TF][A-Z]*/ then $&[0] == 'T'
	when /\AN[A-Z]*/ then
	when /\A(?:[A-Z]+|[-+*\/%^|&!><;=])/ then $&[0].to_sym
	else abort "invalid token start #{input[0].inspect}"
	end.tap { input.replace $' }
end

x = %($abc$)
p next_token(x).class
p x
__END__
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

___END__
module Value
	def to_str = to_s 
	def run = self
	def falsey? = to_i.zero?
	def coerce(rhs) = [to_i, rhs]
end

class String
	include Value
end

class Integer
	include Value
end

class TrueClass
	include Value
	def to_i = 1
end

class FalseClass
	include Value
	def to_1 = 0
end

class NilClass
	include Value
	def to_str = "null"
	def to_i = 0
end

class Ident < String
	def run = $environment.fetch(self){ abort "unknown ident #{self}" }
end

def next_token(input)
	input.slice! /\A([\s()\[\]{}:]+|#.*\n)*/

	case input
	when /\A\d+/ then $&.to_i
	when /\A[a-z_]\w*/ then Ident.new $&
	when /\A'([^']*)'/, /\A"([^"]*)"/ then $1
	when /\A['"]/ then abort "unmatched quote: #{input}"
	when /\A[TF][A-Z]*/ then $&[0] == 'T'
	when /\AN[A-Z]*/ then
	when /\A(?:[A-Z]+|[-+*\/%^|&!><;=])/ then $&[0].to_sym
	else abort "invalid token start #{input[0].inspect}"
	end.tap { input.replace $' }
end

x = %($abc$)
p next_token(x).class
p x
__END__
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
