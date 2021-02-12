fb(N, 'FizzBuzz') :- fb(N, 'Fizz'), fb(N, 'Buzz').
fb(N, 'Fizz') :- 0 =:= N mod 3.
fb(N, 'Buzz') :- 0 =:= N mod 5.
fb(N, N).

fizzbuzz(101) :- !.
fizzbuzz(N) :- fb(N, O), write(O), nl, !, Z is N + 1, fizzbuzz(Z).

main :- fizzbuzz(14).
%
