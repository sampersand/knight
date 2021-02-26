datatype value
	= Null
	| Boolean of bool
	| Number of int
	| String of string
	| Identifier of string
	| Function of unit

fun to_string
(*use "value.sml";*)
(*(*exportFn: (string * ((string * string list) -> OS.Process.status)) -> unit*)*)
(*(*fun exportFn s (func) = ()*)*)
(*fun parse () = 12*)
(**)
(*(*datatype value*)
(*main = print "A"*)
(*  = Null*)
(*  | Number of int*)
(*  | Boolean of bool*)
(*  | String of string*)
(*  | Identifier of string*)
(*  | Function of Func*)
(**)
(**)*)
