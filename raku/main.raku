#!/usr/bin/env raku
use lib 'lib';
use Knight;

multi sub MAIN(Str :$) is hidden-from-USAGE {
	say $*USAGE;
}

#$*PROGRAM-NAME = $*PROGRAM-NAME.IO.basename;

#| the expression to execute
multi sub MAIN(Str :e(:$expr)) {
	Knight::run $expr;
}

#| the file to execute
multi sub MAIN(Str :f(:$file)) {
	MAIN expr => $file.IO.slurp
}
