run(identifier(_I), _O) :- !, halt(1).
run(ast(_I, _A), _O) :- !, halt(1).
run(S, S).

boolean(true).
boolean(false).
null.

to_string(string(S), S).
to_string(integer(I), O) :- number_codes(I, O).
to_string(boolean(true), "true").
to_string(boolean(false), "false").
to_string(null, "null").

to_integer(string(S), S) :- fail.
to_integer(integer(I), I).
to_integer(boolean(true), 1).
to_integer(boolean(false), 0).
to_integer(null, 0).

to_boolean(string([]), false).
to_boolean(string(_), true).
to_boolean(integer(0), false).
to_boolean(integer(_), true).
to_boolean(boolean(B), B).
to_boolean(null, false).

add(string(L), R, string(O)) :-
	to_string(R, RS),
	append(L, RS, O).
add(L, R, integer(O)) :-
	to_integer(L, LI),
	to_integer(R, RI),
	O is LI + RI.
sub(L, R, integer(O)) :-
	to_integer(L, LI),
	to_integer(R, RI),
	O is LI - RI.
mul(string(L), R, O) :- halt(100).
mul(L, R, integer(O)) :-
	to_integer(L, LI),
	to_integer(R, RI),
	O is LI * RI.
div(L, R, integer(O)) :-
	to_integer(L, LI),
	to_integer(R, RI),
	O is LI // RI.
mod(L, R, integer(O)) :-
	to_integer(L, LI),
	to_integer(R, RI),
	O is LI mod RI.
pow(L, R, integer(O)) :-
	to_integer(L, LI),
	to_integer(R, RI),
	O is integer(LI ** RI).

main :- 
	L = string(10),
	R = integer(3),
	add(L, R, string(Q)),
	%atom_codes(A, Q),
	write(Q).
%
%	Y = kn_string('1Hello, '),
%	X = kn_integer(123),
%	add(X, Y, O),
%	write(O).
	%V = kn_string(true),
	%foo(V).
%	atom_concat('a','b', X),
