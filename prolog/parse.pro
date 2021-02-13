whitespace([C|T], T) :- member(C, " \t\n").
comment([0'#|T], Rest) :- append(_, [0'\n|Rest], T) . % :- .
quote(C) :- member(C, "'\"").
digit(C) :- member(C, "0123456789").
lower(C) :- member(C, "abcdefghijklmnopqrstuvwxyz_").
upper(C) :- member(C, "ABCDEFGHIJKLMNOPQRSTUVWXYZ").
symbol(C) :- member(C, "`!+-*/%^?&|;=<>").


empty([]).
ishead(P, [H|_]) :- call(P, H).

string(string(String), [Quote|Tail], Rest) :-
	quote(Quote),
	append(String, [Quote|Rest], Tail).

identifier(identifier(I), Stream, Rest) :- 
	ishead(lower, Stream),
	append(Ident, Rest, Stream),
	( empty(Rest); \+ishead(lower, Rest), \+ishead(digit, Rest) ),
	atom_codes(I, Ident).

integer(integer(Int), Stream, Rest) :-
	ishead(digit, Stream),
	append(Bytes, Rest, Stream),
	( empty(Rest); \+ishead(digit, Stream) ),
	number_codes(Int, Bytes).

keyword([C|S], R) :- upper(C), keyword(S, R).
keyword(R, R).

boolean(true, [0'T|T], R)  :- keyword(T, R).
boolean(false, [0'F|T], R) :- keyword(T, R).
null(null, [0'N|T], R)     :- keyword(T, R).

function(V, [C|S], Rest) :-
	(
		symbol(C), S2 = S;
		upper(C), append(_, S2, S), ( empty(S2) ; \+ishead(upper, S2) )
	function_(V, C, S2, Rest).

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
	Stream = "#1\n NUL LABC 456a",
	expr(V, Stream, Out),
	!,
	nl,write(V), 
	nl,write(Out),nl
	.
