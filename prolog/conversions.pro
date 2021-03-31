% HELPER FUNCTIONS
:- include('helpers.pro').

null('NULL').
boolean('TRUE').
boolean('FALSE').
string(T) :- list(T). % effectively an alias.

% PROGRAM
boolean('FALSE', 'FALSE').
boolean('NULL', 'FALSE').
boolean(0, 'FALSE').
boolean("", 'FALSE').
boolean(N, 'TRUE') :- number(N); string(N); boolean(N).
boolean(Other, Bool) :- run(Other, Tmp), boolean(Tmp, Bool).

string('TRUE', "true").
string('FALSE', "false").
string('NULL', "null").
string(Num, Str) :- number(Num), number_codes(Num, Str).
string(Str, Str) :- string(Str).
string(Other, Str) :- run(Other, Tmp), !, string(Tmp, Str).

number('TRUE', 1).
number('FALSE', 0).
number('NULL', 0).
number(Num, Num) :- number(Num).
number(Str, Num) :- % TODO: negative number conversions
	string(Str),
	append(_, Tmp, Str),
	\+ishead(whitespace, Tmp), !, % first time we dont see whitespace, stop there.
	append(Bytes, Rest, Tmp),
	\+ishead(digit, Rest), !, % once we see the first nondigit, we're done.
	(
		Bytes = [], Num is 0;
		number_codes(Num, Bytes)
	).
number(Other, Str) :- run(Other, Tmp), !, number(Tmp, Str).


run(N, N) :- string(N); number(N); boolean(N); null(N).
run(_N, _Result) :- error("todo run other things").