#!/usr/bin/env raku

use lib $*PROGRAM.parent.add('lib');
use Knight;

@*ARGS = @*ARGS.join('=') if @*ARGS.elems == 2;

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
