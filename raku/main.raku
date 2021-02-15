use lib 'lib';
use Knight;
use Knight::Null;

say Knight::Null.new;
=finish
multi sub MAIN(Str :$ef) is hidden-from-USAGE {
	say $*USAGE;
}

#$*PROGRAM-NAME = $*PROGRAM-NAME.IO.basename;

#| the expression to execute
multi sub MAIN(Str :e(:$expr)) {
	Knight::run $expr;
}

#| the file to execute
multi sub MAIN(IO::Path :f(:$file)) {
	MAIN expr => $file.slurp
}
