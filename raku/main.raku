grammar Knight {
	
}
# grammar Knight {
# 	rule TOP {
# 		.*
# 	}
# }
# 
# say Knight.parse(q/a/);

grammar Calculator {
    token TOP { [ <add> | <sub> ] }
    rule  add { <num> '+' <num> }
    rule  sub { <num> '-' <num> }
    token num { \d+ }
}

say Calculator.parse: '2 + 3 ';

# =>
# ｢2+3｣
#  add => ｢2+3｣
#   num => ｢2｣
#   num => ｢3｣

 #
#class Calculations {
#    method TOP ($/) { make $<add> ?? $<add>.made !! $<sub>.made; }
#    method add ($/) { make [+] $<num>; }
#    method sub ($/) { make [-] $<num>; }
#}
 #
#, actions => Calculations).made;
 #
## OUTPUT: «5␤»
