use Knight;
use Knight::Value;
use Knight::String;
use Knight::Boolean;
use Knight::Number;
use Knight::Null;
use Knight::NonIdempotent;

#| The function type within Knight.
#|
#| Functions are registered via the `register` function.
unit class Knight::Function does Knight::Value does Knight::NonIdempotent;

has $!func is built;
has @!args is built;

#`(
my %FUNCS;

sub register(&func) is export {
	say &func.gist;
	exit;
	%FUNCS{&func.^name} = &func;
}

register anon sub «nullary»{ Knight::String.new: get };
)
my %FUNCS = (
	'P' => { Knight::String.new: get },
	'R' => { Knight::Number.new: (^0xffff_ffff).pick },
	'E' => { Knight::run .Str },
	'B' => { $_ },
	'C' => { .run.run },
	'`' => { Knight::String.new: qqx<$_> },
	'Q' => &exit,
	'!' => { Knight::Boolean.new: !$_ },
	'L' => { Knight::Number.new: .Str.chars },
	'O' => {
		my $result = $^a.run;
		my $to-output = $result.Str;

		if $to-output.substr(* - 1) eq '\\' {
			print $to-output.substr(0, * - 1);
		} else {
			say $to-output;
		}

		$result;
	},

	'+' => { $^a.run.add: $^b.run },
	'-' => { $^a.run.sub: $^b.run },
	'*' => { $^a.run.mul: $^b.run },
	'/' => { $^a.run.div: $^b.run },
	'%' => { $^a.run.mod: $^b.run },
	'^' => { $^a.run.pow: $^b.run },
	'<' => { Knight::Boolean.new: $^a.run.lth: $^b.run },
	'>' => { Knight::Boolean.new: $^a.run.gth: $^b.run },
	'?' => { Knight::Boolean.new: $^a.run.eql: $^b.run },
	'&' => { $^a.run and $^b.run },
	'|' => { $^a.run or $^b.run },
	'=' => { $^a.assign: $^b },
	';' => { $^a.run; $^b.run; },

	'W' => {
		my $ret = Knight::Null.new;

		$ret = $^b.run while $^a;

		$ret;
	},

	'I' => { $^a ?? $^b.run !! $^c.run },
	'G' => { Knight::String.new: $^a.Str.substr($^b.Int, $^c.Int) },

	'S' => {
		my $tostr = $^a.Str.clone;

		$tostr.substr-rw($^b.Int, $^c.Int) = $^d.Str;
		say "<$tostr>";

		Knight::String.new: $tostr
	},
);

method new($name, *@args) {
	die "unknown function '$name'" unless $name ~~ %FUNCS;

	self.bless: :@args, func => %FUNCS{$name}
}

method run(--> Knight::Value) {
	$!func(|@!args)
}
