unit module Knight;

use MONKEY-SEE-NO-EVAL;

EVAL qx<cat classes.raku>;

grammar Syntax {
	rule TOP { <expr> .* }
	rule expr {
		<.ws> <(<literal> | <function>)>
	}

	regex ws {
		[ <[\s(){}[\]:]>+ | '#' \N* ]*
	}

	proto token literal { * }
	token literal:sym«variable» { <[a..z_]> <[a..z0..9_]>* }
	token literal:sym«number»   { \d+ }
	token literal:sym«string»   { '"' .*? '"' | "'" .*? "'" }
	token literal:sym«null»     { <('N')> <.kw-fun-end> }
	token literal:sym«boolean»  { <(<[TF]>)> <.kw-fun-end> }

	token kw-fun-end { <[A..Z]>* }

	proto token function { * }
}

say Syntax.parse("  NULL");
