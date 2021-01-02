#!/usr/bin/awk -f

# Collect source code.
{ source_code = source_code "\n" $0 }

# Execute source code.
END {
	$0 = source_code
	srand()
	run(generate_ast())
}


# Prints a message and then exits.
function die(msg) {
	print msg
	exit 1
}

# used internally to indicate a bug has occured.
function bug(msg) {
	die("bug: " msg)
}

# NOTE: awk starts indexing at 1

# Fetch the next token from `$0`, updating `$0` in the process. Returns a falsey
# value if the stream is empty, and `die`s if an invalid character is
# encountered.
function next_token() {
	# Strip out all leading whitespace and comments
	while (sub(/^([][[:blank:]\n(){}:]+|#[^\n]*\n)/, "")) {
		# do nothing
	}

	# If `$0`'s empty, then return.
	if (!length())
		return

	if (match($0, /^[0-9]+/))
		token = "n" substr($0, 1, RLENGTH)
	else if (match($0, /^[a-z_][a-z0-9_]*/))
		token = "i" substr($0, 1, RLENGTH)
	else if (match($0, /^("[^"]*"|'[^']*')/))
		token = "s" substr($0, 2, RLENGTH - 2) # strip out the quotes
	else if (match($0, /^([[:upper:]]+|[-`+*\/%^<>&|!;=])/))
		token = "f" substr($0, 1, 1) # ignore everything but first char for keywords
	else
		die("unknown token start '" substr($0, 1, 1) "'")

	$0 = substr($0, RLENGTH + 1)
	return token
}

# Generates an AST tree, storing values in the `ast` global variable. Returns
# the index of the node that was just generated.
# (The two parameters are simply local variables)
function generate_ast(node_idx, token) {
	if (!(token = next_token()))
		return

	ast[node_idx = next_node_idx += 1] = token

	if (substr(token, 1, 1) != "f" || token ~ /f[TFNPR]/)
		return node_idx

	if (!(ast[node_idx, 1] = generate_ast()))
		die("missing first argument for function '" substr(token, 2, 1) "'")
	if (token ~ /f[`OEBCQ!L]/)
		return node_idx

	if (!(ast[node_idx, 2] = generate_ast()))
		die("missing second argument for function '" substr(token, 2, 1) "'")
	if (token !~ /f[IGS]/)
		return node_idx

	if (!(ast[node_idx, 3] = generate_ast()))
		die("missing third argument for function '" substr(token, 2, 1) "'")
	if (token !~ /f[S]/)
		return node_idx

	if (!(ast[node_idx, 4] = generate_ast()))
		die("missing fourth argument for function '" substr(token, 2, 1) "'")

	return node_idx
}

# converts `input` to a string, `run`ning the value first unless `dontrun` is
# given.
function to_string(input, dontrun) {
	if (!dontrun) input = run(input)

	if (input ~ /^[sn]/) return substr(input, 2)
	if (input == "fT") return "true"
	if (input == "fF") return "false"
	if (input == "fN") return "null"

	bug("bad input for 'to_string': '" input "'")
}

# converts `input` to a number, `run`ning the value first unless `dontrun` is
# given.
function to_number(input, dontrun) {
	if (!dontrun) input = run(input)

	if (input ~ /^[sn]/) return int(substr(input, 2))
	if (input == "fT") return 1
	if (input ~ /f[FN]/) return 0
	bug("bad input for 'to_number': '" input "'")
}

# converts `input` to a boolean, `run`ning the value first unless `dontrun` is
# given.
function to_boolean(input, dontrun) {
	if (!dontrun) input = run(input)

	if (input ~ /^s/) return length(input) != 1
	if (input ~ /^n/) return input != "n0"
	if (input ~ /^f[TFN]/) return input == "fT"
	bug("bad input for 'to_boolean': '" input "'")
}

# Runs the ast node for `node_idx`, returning the value that's computed.
# All other variables in the function declaration are actually local variables,
function run(node_idx, token, fn, tmp, arg1, arg2, arg3, arg4) {
	if (!(node_idx in ast))
		bug("token number '" node_idx "' doesn't exist in the ast")

	# if it's a literal, return the token directly.
	if ((token = ast[node_idx]) ~ /^([sn]|f[TFN])/)
		return token
	
	# if it's an identifier, evaluate it.
	if (substr(token, 1, 1) == "i") {
		if (token in env)
			return env[token]
		else
			die("variable '" substr(token, 2) "' not found!")
	}
	
	# alias to make string comparisons slightly faster. 
	fn = substr(token, 2)

	# Get a new input line, store it into tmp, and then return that value.
	if (fn == "P") {
		getline tmp
		return "s" tmp
	}

	# At this point, all remaining functions have at least 1 argument.
	arg1 = ast[node_idx, 1]

	# Logical negation. All arguments are converted to booleans first.
	if (fn == "!") {
		return to_boolean(arg1) ? "fF" : "fT"
	}

	# Output something to stdout. If the string ends in `\`, a newline won't be
	# added. We return the evaluated argument.
	if (fn == "O") {
		if ((str = to_string(arg1 = run(arg1), 1)) ~ /\\$/)
			printf "%s", substr(str, 1, length(str) - 1)
		else
			print str

		return arg1
	}

	# When creating blocks, simply return the token number of the thing to eval.
	if (fn == "B")
		return arg1

	# When calling a block, you need to execute the result of running `arg1`.
	if (fn == "C")
		return run(run(arg1))

	# Evaluates the first argument as Knight code, returning the result of the
	# evaluation.
	if (fn == "E") {
		$0 = to_string(arg1)
		return run(generate_ast())
	}

	# Quit exits with the status code of its argument.
	if (fn == "Q") 
		exit to_number(arg1)

	# Gets the length of the first argument, in chars.
	if (fn == "L") 
		return "n" length(to_string(arg1))

	# The '`' keyword's used to execute shell commands and get the stdout.
	if (fn == "`") {
		#to_string(arg1) | getline
		tmp = ""

		while (to_string(arg1) | getline arg2)
			tmp = tmp arg2 "\n"

		return "s" tmp
	}

	# At this point, all remaining functions have at least 2 arguments.
	arg2 = ast[node_idx, 2]

	# The `;` function simply evaluates the LHS then the RHS, returning the RHS.
	if (fn == ";") {
		run(arg1)
		return run(arg2)
	}

	# The `=` operator assigns to the global scope then return the assigned val.
	if (fn == "=") {
		return env[ast[arg1]] = run(arg2)
	}

	# The `+` operator: If the first operand's a string, we do concatenation.
	# Otherwise, we do numerical addition.
	if (fn == "+") {
		if (substr(arg1 = run(arg1), 1, 1) == "s") 
			return "s" to_string(arg1, 1) to_string(arg2)
		else
			return "n" (to_number(arg1, 1) + to_number(arg2))
	}

	# Normal math ops
	if (fn == "-")  return "n" (to_number(arg1) -  to_number(arg2))
	if (fn == "*")  return "n" (to_number(arg1) *  to_number(arg2))
	if (fn == "/")  return "n" (to_number(arg1) /  to_number(arg2))
	if (fn == "%")  return "n" (to_number(arg1) %  to_number(arg2))
	if (fn == "**") return "n" (to_number(arg1) ** to_number(arg2))

	# Short-circuiting logical operators.
	if (fn ~ /[&|]/) 
		return to_boolean(arg1 = run(arg1), 1) == (fn == "&") ? run(arg2) : arg1

	# Comparison operators. We don't have to do anything fancy for string/numebrs
	# as awk already does that for us.
	if (fn ~ /[<>]/) {
		if (substr(arg1 = run(arg1), 1, 1) == "s")  {
			arg1 = to_string(arg1, 1)
			arg2 = to_string(arg2)
		} else {
			arg1 = to_number(arg1, 1)
			arg2 = to_number(arg2)
		}
		return (fn == "<" ? arg1 < arg2 : arg1 > arg2) ? "fT" : "fF"
	}

	# The while function executes the body until the condition is false. The
	# return value is `null` if body never runs, otherwise it's the last returned
	# value from the body.
	if (fn == "W") {
		tmp = "fN"

		while (to_boolean(arg1))
			tmp = run(arg2)

		return tmp
	}

	# Randomly pick an integer from 0 to 0xff_ff_ff_ff
	if (fn == "R") {
		return "n" int(rand() * 4294967295)
	}

	# At this point, all remaining functions have at least 3 arguments.
	arg3 = ast[node_idx, 3]

	# Runs either the second or third argument, depending on the first argument.
	if (fn == "I")
		return run(to_boolean(arg1) ? arg2 : arg3)

	if (fn == "G")
		return "s" substr(to_string(arg1), to_number(arg2) + 1, to_number(arg3))

	# At this point, all remaining functions have at least 4 arguments.
	arg4 = ast[node_idx, 4]

	if (fn == "S") {
		arg1 = to_string(arg1)
		arg2 = to_number(arg2)
		arg3 = to_number(arg3)
		arg4 = to_string(arg4)
		return "s" substr(arg1, 1, arg2) arg4 substr(arg1, arg2 + arg3, length(arg1))
	}

	bug("unknown function to evaluate '" fn "'")
}
