#!/usr/bin/env raku
use lib 'lib';
use Knight;

#signal(SIGINT).tap( { say Backtrace.new.summary; exit 0 } );

multi sub MAIN(Str :$) is hidden-from-USAGE {
	say $*USAGE;
}

#| the expression to execute
multi sub MAIN(Str :e(:$expr)) {
	Knight::run $expr;
}

#| the file to execute
multi sub MAIN(Str :f(:$file)) {
	MAIN expr => $file.IO.slurp
}
