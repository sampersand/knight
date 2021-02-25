use Knight::Value;
use Knight::Boolean;
use Knight::Null;
use Knight::Number;
use Knight::String;
use Knight::Identifier;
use Knight::Function;

#| The module that contains all things for parsing Knight programs.
unit module Knight::Parser;

#| The Syntax of Knight.
grammar Syntax {
	#| A knight program is a single expression, followed by any amount of characters.
	rule TOP { <expr> .* }

	#| An expression is any amount of whitespace, followed by a literal or a function call.
	rule expr { <.ws> [<literal> | <function>] }

	#| Whitespace in Knight is:
	#| - Normal whitespace and newlines (`\s` and `\n`)
	#| - All parenthesis (`(`, `)`, `{`, `}`, `[`, and `]`) and the colon `:`.
	#| - Comments, which start with a `#` and go until end of line.
	token ws { [ <[(){}[\]:\s\n]> | '#' \N* \n? ]* }

	#| A helper token used to discard trailing uppercase letters in keyword functions.
	token kw-rest { <[A..Z]>* }

	#| Literal tokens in Knight are:
	#| - Identifiers, which start with a lower case letter, or `_`, followed by any amount of lower-case letters, `_`,
	#|   or digits.
	#| - Numbers, which are just a sequence of digits.
	#| - Strings, which are a quote (`'` or `"`), followed by any amount of characters until the same quote is
	#|   encountered. Note that there are absolutely no escapes whatsoever.
	#| - Booleans, which are just `T` or `F` followed by any amount of upper-case letters,
	#| - Null, which is just `N` followed by any amount of upper-case letters.
	proto token literal { * }
	      token literal:sym«identifier» { <[a..z_]> <[a..z0..9_]>* }
	      token literal:sym«number»     { \d+ }
	      token literal:sym«string»     { '"' [.|\n]*? '"' | "'" [.|\n]*? "'" }
	      token literal:sym«null»       { 'N' <.kw-rest> }
	      token literal:sym«boolean»    { (<[TF]>) <.kw-rest> }

	#| In Knight, everything that is not a literal is a function, including things like `;` and `=`.
	#|
	#| Functions all have a known arity, and as such they can easily be parsed: `IF x (OUTPUT "yup") (OUTPUT "nop")`.
	#|
	#| There are two types of functions: Symbolic ones (such as `+`, `?`, `!`, etc) which are exactly a single character,
	#| and keyword functions. Keyword functions are identified by their first character, and may have any amount of
	#| upper-case letters following it.
	proto rule function { * }

	#| Nullary functions take no arguments.
	rule function:«nullary»         { <nullary> }
	proto token nullary             { * }
	      token nullary:sym«prompt» { 'P' <.kw-rest> }
	      token nullary:sym«random» { 'R' <.kw-rest> }

	#| Unary functions take a single argument.
	rule function:«unary»         { <unary> <expr> }
	proto token unary             { * }
	      token unary:sym«eval»   { 'E' <.kw-rest> }
	      token unary:sym«block»  { 'B' <.kw-rest> }
	      token unary:sym«call»   { 'C' <.kw-rest> }
	      token unary:sym«system» { '`' }
	      token unary:sym«quit»   { 'Q' <.kw-rest> }
	      token unary:sym«not»    { '!' }
	      token unary:sym«length» { 'L' <.kw-rest> }
	      token unary:sym«dump»   { 'D' <.kw-rest> }
	      token unary:sym«output» { 'O' <.kw-rest> }

	#| Binary functions take two arguments.
	rule function:«binary»        { <binary> <expr> <expr> }
	proto token binary            { * }
	      token binary:sym«+»     { <sym> }
	      token binary:sym«-»     { <sym> }
	      token binary:sym«*»     { <sym> }
	      token binary:sym«/»     { <sym> }
	      token binary:sym«%»     { <sym> }
	      token binary:sym«^»     { <sym> }
	      token binary:sym«?»     { <sym> }
	      token binary:sym«&»     { <sym> }
	      token binary:sym«|»     { <sym> }
	      token binary:sym«;»     { <sym> }
	      token binary:sym«=»     { <sym> }
	      token binary:sym«<»     { <sym> }
	      token binary:sym«>»     { <sym> }
	      token binary:sym«while» { 'W' <.kw-rest> }

	#| Ternary functions take three arguments.
	rule function:«ternary»      { <ternary> <expr> <expr> <expr> }
	proto token ternary          { * }
	      token ternary:sym«if»  { 'I' <.kw-rest> }
	      token ternary:sym«get» { 'G' <.kw-rest> }

	#| Quatenary functions take four arguments.
	rule function:«quatenary»      { <quatenary> <expr> <expr> <expr> <expr> }
	proto token quatenary          { * }
	      token quatenary:sym«set» { 'S' <.kw-rest> }
# previous attempts at better error handling:
#	method missing-arg($token, $index) {
#		my $lineno = self.target.substr(0, self.pos).lines.elems;
#		die "[Line '$lineno'] Missing argument number $index for token '$token'";
#	}

#	method expr-index($index) {
#		/ <expr> / # || <missing-arg("?", $index)> /
#		; /<expr>/
#	}
}

#| Actions that happen with `Syntax`.
#|
#| These simply construct the AST, and dont anything fancy.
class SyntaxAction {
	method TOP($/)  { make $<expr>.made; }
	method expr($/) { make $/.values[0].made; }

	method literal:sym«identifier»($/) { make Knight::Identifier.new: ~$/ }
	method literal:sym«number»($/)     { make Knight::Number.new: +$/ }
	method literal:sym«string»($/)     { make Knight::String.new: $/.Str.substr(1, *-1) }
	method literal:sym«null»($/)       { make Knight::Null.new }
	method literal:sym«boolean»($/)    { make Knight::Boolean.new: $/[0] eq 'T' }

	method function:«nullary»($/)  { make Knight::Function.new: $<nullary>.made }
	method nullary:sym«prompt»($/) { make 'P' }
	method nullary:sym«random»($/) { make 'R' }

	method function:«unary»($/)  { make Knight::Function.new: $<unary>.made, $<expr>.made }
	method unary:sym«eval»($/)   { make 'E' }
	method unary:sym«block»($/)  { make 'B' }
	method unary:sym«call»($/)   { make 'C' }
	method unary:sym«system»($/) { make '`' }
	method unary:sym«quit»($/)   { make 'Q' }
	method unary:sym«not»($/)    { make '!' }
	method unary:sym«length»($/) { make 'L' }
	method unary:sym«dump»($/)   { make 'D' }
	method unary:sym«output»($/) { make 'O' }

	method function:«binary»($/) { make Knight::Function.new: $<binary>.made, |$<expr>».made }
	method binary:sym«+»($/)     { make ~$<sym> }
	method binary:sym«-»($/)     { make ~$<sym> }
	method binary:sym«*»($/)     { make ~$<sym> }
	method binary:sym«/»($/)     { make ~$<sym> }
	method binary:sym«%»($/)     { make ~$<sym> }
	method binary:sym«^»($/)     { make ~$<sym> }
	method binary:sym«?»($/)     { make ~$<sym> }
	method binary:sym«&»($/)     { make ~$<sym> }
	method binary:sym«|»($/)     { make ~$<sym> }
	method binary:sym«;»($/)     { make ~$<sym> }
	method binary:sym«=»($/)     { make ~$<sym> }
	method binary:sym«<»($/)     { make ~$<sym> }
	method binary:sym«>»($/)     { make ~$<sym> }
	method binary:sym«while»($/) { make 'W' }

	method function:«ternary»($/) { make Knight::Function.new: $<ternary>.made, |$<expr>».made }
	method ternary:sym«if»($/)    { make 'I' }
	method ternary:sym«get»($/)   { make 'G' }

	method function:«quatenary»($/) { make Knight::Function.new: $<quatenary>.made, |$<expr>».made }
	method quatenary:sym«set»($/)   { make 'S' }
}

#| Parses a stringy input into Knight code, and then executes it.
sub parse-and-run($input --> Knight::Value) is export {
	my $func = Syntax.parse(~$input, actions => SyntaxAction).made;

	die 'Syntax error encountered somewhere.' unless defined $func;

	$func.run
}
