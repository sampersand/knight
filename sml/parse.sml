exception CantParse of string

fun parse "" = raise CantParse "whoops"
  | parse (#'a'::rest)  = n;

print ( parse("abc") ^ "\n")