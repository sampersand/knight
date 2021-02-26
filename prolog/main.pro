% HELPER FUNCTIONS
whitespace([C|T], T) :- member(C, " \t\n").
quote(C) :- member(C, "\"\'"). 
digit(C) :- member(C, "0123456789").
lower(C) :- member(C, "abcdefghijklmnopqrstuvwxyz_").
upper(C) :- member(C, "ABCDEFGHIJKLMNOPQRSTUVWXYZ").
symbol(C) :- member(C, "`!+-*/%^?&|;=<>").

% whter the input is empty.

% Ensure the head of 
ishead(P, [H|_]) :- call(P, H).

% Parse a string out.
string(string(String), [Quote|Tail], Rest) :-
	quote(Quote),
	append(String, [Quote|Rest], Tail).

% Parse an integer out.
integer(integer(Int), Stream, Rest) :-
	ishead(digit, Stream),
	append(Bytes, Rest, Stream),
	(Rest = []; \+ishead(digit, Stream)),
	write(Bytes),
	number_codes(Int, Bytes).

identifier(identifier(Ident), Stream, Rest) :- 
	ishead(lower, Stream),
	append(Bytes, Rest, Stream),
	( Rest = []; \+ishead(lower, Rest), \+ishead(digit, Rest) ),
	atom_codes(Ident, Bytes).

% comment([0'#|T], Rest) :- append(_, [0'\n|Rest], T) . % :- .

main :-
	integer(integer(S), "1234 'foo'bar", R), !,
	atom_codes(Str, S),
	atom_codes(Rst, R),
	nl, write(Str), write(':'), write(Rst), nl.
%	quote(0'\"), !,
%	quote(0'\'), !.


%
%
%empty([]).
%ishead(P, [H|_]) :- call(P, H).
%
%keyword([C|S], R) :- upper(C), keyword(S, R).
%keyword(R, R).
%
%boolean(true, [0'T|T], R)  :- keyword(T, R).
%boolean(false, [0'F|T], R) :- keyword(T, R).
%null(null, [0'N|T], R)     :- keyword(T, R).
%
%function(V, [C|S], Rest) :-
%	(
%		symbol(C), S2 = S;
%		upper(C), append(_, S2, S), ( empty(S2) ; \+ishead(upper, S2) )
%	function_(V, C, S2, Rest).
%
%% Strip whitespace.
%expr(Value) --> 
%	whitespace, expr(Value);
%	comment, expr(Value);
%	integer(Value);
%	boolean(Value);
%	string(Value);
%	null(Value);
%	identifier(Value).

main1 :-
	%Stream = "#1\n NUL LABC 456a",
	%expr(V, Stream, Out),
	%!,
	nl,write(V), 
	nl,write(Out),nl
	.
