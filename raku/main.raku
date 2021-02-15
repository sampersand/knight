#!/usr/bin/env raku
use lib 'lib';
use Knight;

#say Knight::run '; = a B O 3 : O + 3 C a';

#=finish
multi sub MAIN(Str :$ef) is hidden-from-USAGE {
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
