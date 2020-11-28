sub run_ast(@ast) {
	given @ast[0] {
		when Number, Array, String, Boolean, Null { @ast[0] }
		when "+" { run_ast(@ast[1]) + run_ast(@ast[2]) }
		when "-" { run_ast(@ast[1]) - run_ast(@ast[2]) }
		when "*" { run_ast(@ast[1]) * run_ast(@ast[3]) }
		...
	}
}
sub op_add($a, $b) { $a + $b }
