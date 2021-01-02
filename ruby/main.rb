require_relative 'value'
Kn::Value.parse(<<EOS).run
; = fizzbuzz BLOCK
	; = n 0
	: WHILE < (= n + 1 n) (+ max 1)
		: OUTPUT
			: IF ! (% n 15)
				: "FizzBuzz"
			: IF ! (% n 5)
				: "Fizz"
			: IF ! (% n 3)
				: "Buzz"
				: n
; = max 100
: CALL fizzbuzz
EOS
