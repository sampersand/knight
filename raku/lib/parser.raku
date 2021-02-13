unit module Knight;

grammar Syntax {
	rule TOP { <expr> .* }
	rule expr { <.ws> [<literal> | <function>] }

	token ws { [ <[(){}[\]:\s\n]> | '#' \N* \n? ]* }
	token kw-rest { <[A..Z]>* }

	proto token literal { * }
	      token literal:sym«identifier» { <[a..z_]> <[a..z0..9_]>* }
	      token literal:sym«number»     { \d+ }
	      token literal:sym«string»     { '"' .*? '"' | "'" .*? "'" }
	      token literal:sym«null»       { 'N' <.kw-rest> }
	      token literal:sym«boolean»    { (<[TF]>) <.kw-rest> }

	proto rule function { * }

	rule function:«nullary»         { <nullary> }
	proto token nullary             { * }
	      token nullary:sym«prompt» { 'P' <.kw-rest> }
	      token nullary:sym«random» { 'R' <.kw-rest> }

	rule function:«unary»         { <unary> <expr> }
	proto token unary             { * }
	      token unary:sym«eval»   { 'E' <.kw-rest> }
	      token unary:sym«block»  { 'B' <.kw-rest> }
	      token unary:sym«call»   { 'C' <.kw-rest> }
	      token unary:sym«system» { '`' }
	      token unary:sym«quit»   { 'Q' <.kw-rest> }
	      token unary:sym«not»    { '!' }
	      token unary:sym«length» { 'L' <.kw-rest> }
	      token unary:sym«output» { 'O' <.kw-rest> }

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

	rule function:«ternary»      { <ternary> <expr> <expr> <expr> }
	proto token ternary          { * }
	      token ternary:sym«if»  { 'I' <.kw-rest> }
	      token ternary:sym«get» { 'G' <.kw-rest> }

	rule function:«quatenary»      { <quatenary> <expr> <expr> <expr> <expr> }
	proto token quatenary          { * }
	      token quatenary:sym«set» { 'S' <.kw-rest> }
#
#	method missing-arg($token, $index) {
#		my $lineno = self.target.substr(0, self.pos).lines.elems;
#		die "[Line '$lineno'] Missing argument number $index for token '$token'";
#	}

#	method expr-index($index) {
#		/ <expr> / # || <missing-arg("?", $index)> /
#		; /<expr>/
#	}
}

class SyntaxAction {
	method TOP($/)  { make $<expr>.made; }
	method expr($/) { make $/.values[0].made; }

	method literal:sym«identifier»($/) { make Knight::Identifier.new: $/.Str }
	method literal:sym«number»($/)     { make Knight::Number.new: $/.Int }
	method literal:sym«string»($/)     { make Knight::String.new: $/.Str.substr(1, *-1) }
	method literal:sym«null»($/)       { make Knight::Null.new }
	method literal:sym«boolean»($/)    { make Knight::Boolean.new: $/[0] eq 'T' }

	method function:«nullary»($/)  { make Knight::Function.new: $<nullary>.made }
	method nullary:sym«prompt»($/) { make 'P' }
	method nullary:sym«random»($/) { make 'R' }

	method function:«unary»($/)  { make Knight::Function.new: $<unary>.made, |[$<expr>]».made }
	method unary:sym«eval»($/)   { make 'E' }
	method unary:sym«block»($/)  { make 'B' }
	method unary:sym«call»($/)   { make 'C' }
	method unary:sym«system»($/) { make '`' }
	method unary:sym«quit»($/)   { make 'Q' }
	method unary:sym«not»($/)    { make '!' }
	method unary:sym«length»($/) { make 'L' }
	method unary:sym«output»($/) { make 'O' }

	method function:«binary»($/) { make Knight::Function.new: $<binary>.made, |$<expr>».made }
	method binary:sym«+»($/)     { make $<sym>.Str }
	method binary:sym«-»($/)     { make $<sym>.Str }
	method binary:sym«*»($/)     { make $<sym>.Str }
	method binary:sym«/»($/)     { make $<sym>.Str }
	method binary:sym«%»($/)     { make $<sym>.Str }
	method binary:sym«^»($/)     { make $<sym>.Str }
	method binary:sym«?»($/)     { make $<sym>.Str }
	method binary:sym«&»($/)     { make $<sym>.Str }
	method binary:sym«|»($/)     { make $<sym>.Str }
	method binary:sym«;»($/)     { make $<sym>.Str }
	method binary:sym«=»($/)     { make $<sym>.Str }
	method binary:sym«<»($/)     { make $<sym>.Str }
	method binary:sym«>»($/)     { make $<sym>.Str }
	method binary:sym«while»($/) { make 'W' }

	method function:«ternary»($/) { make Knight::Function.new: $<ternary>.made, |$<expr>».made }
	method ternary:sym«if»($/)    { make 'I' }
	method ternary:sym«get»($/)   { make 'G' }

	method function:«quatenary»($/) { make Knight::Function.new: $<quatenary>.made, |$<expr>».made }
	method quatenary:sym«set»($/)   { make 'S' }
}

sub run($input --> Value) {
	my $stream = $input.Str;	
	my $func = Syntax.parse($stream, actions => SyntaxAction).made;

	die 'Syntax error encountered somewhere.' unless $func;
	$func.run;
}

#run "; = + 'a' 'b' 2 : OUTPUT ab";

run qqx<cat ../knight.kn>.Str;
=begin comment
my $stream = q:to/EOS/;
	; = fizzbuzz BLOCK
		; = n 0
		; = max + 1 max
		: WHILE < (= n + 1 n) max
		:	OUTPUT
		:		IF ! (% n 15)
		:			"FizzBuzz"
		: 	IF ! (% n 5)
		:			"Fizz"
		:		IF ! (% n 3)
		:			"Buzz"
		:			n
	; = max 100
	: CALL fizzbuzz
#	; O 9
#	: Q 0
#	; = x BLOCK 3
#	; OUTPUT CALL x
#	; Q 0
#	; = a 0
#	; = max + 1 max
#	; OUTPUT a
#	: OUTPUT + "A" 3
EOS
=end comment

#my $func = Syntax.parse($stream, actions => SyntaxAction).made;
#
#$func.run;
##say $func;
