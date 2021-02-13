whitespace([C|T], T) :- member(C, " \t\n").
comment([0'#|T], R) :- append(_, [0'\n|R], T) . % :- .
quote(C) :- member(C, "'\"").
digit(C) :- member(C, "0123456789").
upper(C) :- member(C, "ABCDEFGHIJKLMNOPQRSTUVWXYZ").
lower(C) :- member(C, "abcdefghijklmnopqrstuvwxyz_").


empty([]).

number(integer(V), [D|S], R) :-
	digit(D),
	append(Bytes, R, S),
	(empty(R); R = [N|_], \+digit(N)),
	number_codes(V, [D|Bytes]).

keyword(S, R) :-
	append(_, R, S),
	(empty(R); R = [N|_], \+upper(N)).

boolean(true, [0'T|T], R)  :- keyword(T, R).
boolean(false, [0'F|T], R) :- keyword(T, R).
null(null, [0'N|T], R)     :- keyword(T, R).
string(string(V), [Q|T], R) :- quote(Q), append(V, [Q|R], T).



% Strip whitespace.
expr(V) --> 
	whitespace, expr(V);
	comment, expr(V);
	number(V);
	boolean(V);
	string(V);
	null(V).


main :-
	%Stream = "#1\n T456a",
	Stream = "'TRUE' 3",
	%Stream = [35, 10, 32, 49, 50,51, 52,10,49],
	expr(V, Stream, Out),
	!,
	nl,write(V), 
	nl,write(Out),nl
	.
	%parse(O, _, Q), write(Q).
	%write(Stream1).
	%Z = kn_string([]),
	%write(Z).
