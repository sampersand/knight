# Knight: Perl Edition

I originally thought it'd be fun to have some fancy overloading nonsense for everything, but since that breaks the spirit of some of the operators (eg `<` is intended for numerical comparisons, `.` is for string concatenation, etc), I decided to opt for simply overloading the conversion operators: `""`, `0+`, and `bool`.

