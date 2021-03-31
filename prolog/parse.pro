% HELPER FUNCTIONS
:- include('helpers.pro').

number(Num, Stream, "") :- var(Stream), number_codes(Num, Stream), !.
number(Num, [W|S], Rest) :-
	whitespace(W),
	number(Num, S, Rest).

number(Num, Stream, Rest) :- 
	ishead(digit, Stream),
	append(Bytes, Rest, Stream),
	\+ishead(digit, Rest),
	!,
	number_codes(Num, Bytes).


boolean('TRUE', S, "") :- var(S), S = "true".
boolean('FALSE', S, "") :- var(S), S = "false".
boolean('TRUE',  [0'T|Stream], Rest) :- append(_, Rest, Stream), \+ishead(upper_under, Stream).
boolean('FALSE', [0'F|Stream], Rest) :- append(_, Rest, Stream), \+ishead(upper_under, Stream).

null('NULL', "null", "").
null('NULL',  [0'N|Stream], Rest) :- append(_, Rest, Stream), \+ishead(upper_under, Stream).


main :-
	boolean('TRUE', S, _),
	write(S), nl,
	!.
	%% number(N, "  123a", R),
	%% number(123, S, _),
	%% !,
	%% write(N), nl,
	%% number_codes(N2, S),
	%% write(N2), nl.

%% %% whitespace([C|T], T) :- member(C, " \t\n").
%% %% quote(C) :- member(C, "\"\'"). 
%% %% digit(C) :- member(C, "0123456789").
%% %% lower(C) :- member(C, "abcdefghijklmnopqrstuvwxyz_").
%% %% upper(C) :- member(C, "ABCDEFGHIJKLMNOPQRSTUVWXYZ").
%% %% symbol(C) :- member(C, "`!+-*/%^?&|;=<>").
%% 
%% %% empty([]).
%% 
%% %% % Ensure the head of 
%% %% ishead(P, [H|_]) :- call(P, H).
%% 
%% %% % Parse a string out.
%% %% string(string(String), [Quote|Tail], Rest) :-
%% %% 	quote(Quote),
%% %% 	append(String, [Quote|Rest], Tail).
%% 
%% %% % Parse an integer. Note that `Stream` is guaranteed to end with a non-digit character
%% %% integer(integer(Int), Stream, Rest) :-
%% %% 	ishead(digit, Stream),
%% %% 	append(Bytes, Rest, Stream),
%% %% 	\+ishead(digit, Rest),
%% %% 	number_codes(Int, Bytes).
%% 
%% %% % Parse an identifier
%% %% identifier(identifier(Ident), Stream, Rest) :- 
%% %% 	ishead(lower, Stream),
%% %% 	append(Bytes, Rest, Stream),
%% %% 	\+ishead(lower, Rest),
%% %% 	\+ishead(digit, Rest),
%% %% 	atom_codes(Ident, Bytes).
%% 
%% 
%% %% append_space([H|Rest], Out).
%% 
%% %% parse(Value, Input) :-
%% %% 	append(Input, " ", Stream),
%% %% 	parseInner(Value, Input, _).
%% 
%% %% main :-
%% %% 	integer(integer(S), "1234 'foo'bar", R), !,
%% %% 	atom_codes(Rst, R),
%% %% 	nl, write(S), write(':'), write(Rst), nl.
%% %% %	quote(0'\"), !,
%% %% %	quote(0'\'), !.
%% 
%% 
%% %% %
%% %% %
%% %% %empty([]).
%% %% %ishead(P, [H|_]) :- call(P, H).
%% %% %
%% %% %keyword([C|S], R) :- upper(C), keyword(S, R).
%% %% %keyword(R, R).
%% %% %
%% %% %boolean(true, [0'T|T], R)  :- keyword(T, R).
%% %% %boolean(false, [0'F|T], R) :- keyword(T, R).
%% %% %null(null, [0'N|T], R)     :- keyword(T, R).
%% %% %
%% %% %function(V, [C|S], Rest) :-
%% %% %	(
%% %% %		symbol(C), S2 = S;
%% %% %		upper(C), append(_, S2, S), ( empty(S2) ; \+ishead(upper, S2) )
%% %% %	function_(V, C, S2, Rest).
%% %% %
%% %% %% Strip whitespace.
%% %% %expr(Value) --> 
%% %% %	whitespace, expr(Value);
%% %% %	comment, expr(Value);
%% %% %	integer(Value);
%% %% %	boolean(Value);
%% %% %	string(Value);
%% %% %	null(Value);
%% %% %	identifier(Value).
%% 
%% %% main1 :-
%% %% 	%Stream = "#1\n NUL LABC 456a",
%% %% 	%expr(V, Stream, Out),
%% %% 	%!,
%% %% 	nl,write(V), 
%% %% 	nl,write(Out),nl
%% %% 	.
