whitespace([Chr|Tail], Tail) :- member(Chr, " \t\n").
comment([0'#|Tail], Rest) :- append(_, [0'\n|Rest], Tail) . % :- .
quote(Chr) :- member(Chr, "'\"").
digit(Chr) :- member(Chr, "0123456789").
upper(Chr) :- member(Chr, "ABCDEFGHIJKLMNOPQRSTUVWXYZ").

integer(integer(Int), [Chr|Stream], Rest) :-
	digit(Chr),
	append(Bytes, Rest, Stream),
	(
		Rest == [];
		[NonDigit|_] = Rest, \+digit(NonDigit)
	),
	number_codes(Int, [Chr|Bytes]).

keyword(Stream, Rest) :-
	append(_, Rest, Stream),
	(Rest = []; Rest = [N|_], \+upper(N)).

boolean(true, [0'T|Tail], Rest)  :- keyword(Tail, Rest).
boolean(false, [0'F|Tail], Rest) :- keyword(Tail, Rest).
null(null, [0'N|Tail], Rest)     :- keyword(Tail, Rest).
string(string(String), [Quote|Tail], Rest) :-
	quote(Quote),
	append(String, [Quote|Rest], Tail).

identifier(identifier(I))

% Strip whitespace.
expr(Value) --> 
	whitespace, expr(Value);
	comment, expr(Value);
	integer(Value);
	boolean(Value);
	string(Value);
	null(Value);
	identifier(Value).

main :-
	Stream = "#1\n N456a",
	expr(V, Stream, Out),
	!,
	nl,write(V), 
	nl,write(Out),nl
	.
