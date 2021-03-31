% HELPER FUNCTIONS
:- include('conversions.pro').

prompt(_Result) :- error("todo").
random_(Result) :- random(0, 2147483647, Result). % TODO: seed this?

eval(Arg, Result) :-
	string(Arg, Str), !,
	parse(Value, Str, _), !,
	run(Value, Result).

block(Arg, Arg).
call_(Arg, Result) :- run(Arg, Tmp), !, run(Tmp, Result).
system_(_Arg, _Result) :- error("todo").
quit(Arg) :- number(Arg, Code), halt(Code).
not(Arg, Result) :- 
	boolean(Arg, Bool),
	(Bool = 'TRUE', Result = 'FALSE'; Result = 'TRUE').
length_(Arg, Result) :-
	string(Arg, Str),
	length_helper(Str, Result).
length_helper("", 0).
length_helper([_|Rest], N) :- length_helper(Rest, Tmp), N is 1 + Tmp.
dump(Arg, Result) :-
	run(Arg, Result),
	(	Result = 'TRUE', write('Boolean(true)'), nl
	;	Result = 'FALSE', write('Boolean(false)'), nl
	;	Result = 'NULL', write('Null()'), nl
	;	number(Result), write('Number('), write(Result), write(')'), nl
	;	stirng(Result), write('String('), atom_codes(S, Result), write(S), write(')'), nl
	).

output(Arg, 'NULL') :-
	string(Arg, Str),
	(	output_remove_trailing_slash(Str, T), write(T)
	;	write(Str), nl
	).

output_remove_trailing_slash("\\", "").
output_remove_trailing_slash([C|R], O) :- output_remove_trailing_slash(R, T), O=[C|T].

add(Lhs, Rhs, Result) :- 
	run(Lhs, LRan), !,
	(	number(LRan), !, % If the LRan is a number, this is the function to use.
		number(Rhs, RNum), !, % Take whatever value the rhs gives us.
		Result is LRan + RNum
	;	string(LRan), !,
		string(Rhs, RStr), !,
		append(LRan, RStr, Result)
	;	error("invalid kind given to add")
	).

sub(Lhs, Rhs, Result) :-
	run(Lhs, LRan), !,
	(	number(LRan);
		number(Rhs, RNum), !,
		Result is LRan - RNum
	;	error("invalid kind given to sub")
	).

mul(Lhs, Rhs, Result) :-
	run(Lhs, LRan),
	(	number(LRan), !,
		number(Rhs, RNum), !,
		Result is LRan * RNum
	;	string(LRan), !,
		number(Rhs, RNum), !,
		mul_string_inner(LRan, RNum, Result)
	;	error("invalid value kind to mul")
	).
mul_string_inner(_, 0, "").
mul_string_inner(L, 1, L).
mul_string_inner(L, N, R) :-
	N1 is N - 1,
	mul_string_inner(L, N1, R1),
	append(L, R1, R).

div(Lhs, Rhs, Result) :-
	run(Lhs, LRan), !,
	(	number(LRan), !,
		number(Rhs, RNum), !,
		(RNum = 0, error("division by zero"); pass),
		Result is truncate(LRan / RNum)
	;	error("invalid kind given to div")
	).

mod(Lhs, Rhs, Result) :-
	run(Lhs, LRan), !,
	(	number(LRan), !,
		number(Rhs, RNum), !,
		(RNum = 0, error("modlo by zero"); pass),
		Result is mod(LRan, RNum)
	;	error("invalid kind given to mod")
	).

pow(Lhs, Rhs, Result) :-
	run(Lhs, LRan), !,
	(	number(LRan), !,
		number(Rhs, RNum), !,
		Result is truncate(LRan ** RNum)
	;	error("invalid kind given to pow")
	).

lth(Lhs, Rhs, 'TRUE') :-
	run(Lhs, LRan), !,
	(	number(LRan), !,
		number(Rhs, RNum), !,
		LRan < RNum
	;	string(LRan), !,
		string(Rhs, _RStr), !,
		error("todo")
	;	boolean(LRan), !,
		boolean(Rhs, RBool), !,
		LRan = 'FALSE', RBool = 'TRUE'
	;	error("invalid kind given to lth")
	).
lth(_, _, 'FALSE').

gth(Lhs, Rhs, 'TRUE') :-
	run(Lhs, LRan), !,
	(	number(LRan), !,
		number(Rhs, RNum), !,
		LRan > RNum
	;	string(LRan), !,
		string(Rhs, _RStr), !,
		error("todo")
	;	boolean(LRan), !,
		boolean(Rhs, RBool), !,
		LRan = 'TRUE', RBool = 'FALSE'
	;	error("invalid kind given to gth")
	).
gth(_, _, 'FALSE').

eql(Lhs, Rhs, 'TRUE') :-
	run(Lhs, LRan), !,
	run(Rhs, RRan), !,
	LRan = RRan.
eql(_, _, 'FALSE').

and(Lhs, Rhs, Result) :-
	run(Lhs, LRan),
	(	boolean(LRan, 'TRUE'), run(Rhs, Result)
	;	Result = LRan % If the lhs is falsey, return that.
	).

or(Lhs, Rhs, Result) :-
	run(Lhs, LRan),
	(	boolean(LRan, 'FALSE'), run(Rhs, Result)
	;	Result = LRan % If the lhs is falsey, return that.
	).

then(Lhs, Rhs, Result) :- run(Lhs, _), !, run(Rhs, Result).

assign(Lhs, _Rhs) :- atomic(Lhs), error("todo assign").
assign(_, _) :- error("invalid kind given to assign").

while(Cond, Body, 'NULL') :-
	boolean(Cond, 'TRUE'), run(Body, _), while(Cond, Body)
	; pass.


main :-
	add("12", 34, O),
	%% number("  \n 123a", Str),
	write(O), nl,
	!.

%	4.3.11 [`|`](#4310-unchanged-unevaluated)  
%	4.3.12 [`;`](#4312-unchanged-unchanged)  
%	4.3.13 [`=`](#4313-unevaluated-unchanged)  
%	4.3.14 [`WHILE`](#4314-whileunevaluated-unevaluated)  
%
%	4.4.1 [`IF`](#441-ifboolean-unevaluated-unevaluated)  
%	4.4.2 [`GET`](#442-getstring-number-number)  
%
%	4.5.1 [`SUBSTITUTE`](#451-substitutestring-number-number-string)  
