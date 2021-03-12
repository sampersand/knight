use "parse.sml";
(*fun op++ (x, y) = x + y + y;
infix 6 ++;

1 ++ 2;

map (fn x => 1 ++ x) [1, 2, 3]*)

fun fib 0 = 0
  | fib 1 = 1
  | fib n = fib (n - 1) + fib (n - 2)

(*val x = print string 12;*)
fun printIt (prefix, s) =
  print (Option.getOpt (prefix, "Hello, ") ^ s ^ "\n");

printIt (NONE, "world");
printIt (SOME "1", "2");

print ((Int.toString (fib 40)) ^ "\n")