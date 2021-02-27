#!/usr/bin/env gprolog --consult-file
:- include('helpers.pro').

% Checks to see if an object is a string, ie an array of bytes.
string(L) :- is_list(L), forall(member(X, L), number(X)).

% Converts something to and from a string.
string('True', "true").
string('False', "false").
string('Null', "null").
string(Num, Str) :- (var(Num); integer(Num)), number_helper(Num, Str).
string(Str, Str) :- string(Str). % strings are strings, lol.
%string(Ident, Str) :- atom(Ident), !, run(Ident, X), string(X, Str).
%string(Function, Str) :- function(Function), !, run(Ident, X), string(X, Str).

strip_whitespace([S|In], Out) :- whitespace(S), strip_whitespace()
strip_whitespace(In, In).
number_helper(N, [H|3T]) :-
	digit(H),
	number_helper(X, T),
	N is (H - 48) + X.
number_helper(0, _).

number('True', 1).
number('False', 0).
number('Null', 0).
number(Str, Num) :- number_helper(Num, Str).


main :- 
	number_helper(S, "5"), !,
	%atom_codes(O, S),
	write(S), nl.
	%% string('False', O), !,
	%% atom_codes(S, O),
	%% write(S), nl.

%% %% % HELPER FUNCTIONS
%% %% whitespace([C|T], T) :- member(C, " \t\n").
%% %% quote(C) :- member(C, "\"\'"). 
%% %% digit(C) :- member(C, "0123456789").
%% %% lower(C) :- member(C, "abcdefghijklmnopqrstuvwxyz_").
%% %% upper(C) :- member(C, "ABCDEFGHIJKLMNOPQRSTUVWXYZ").
%% %% symbol(C) :- member(C, "`!+-*/%^?&|;=<>").

%% %% empty([]).
