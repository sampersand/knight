%% Whitespace is any of `\t\n\v\f\r`.
whitespace(C) :- 9 =< C, C =< 13. % `\t\n\v\f\r`
whitespace(32). % space

%% A Quote is either a single or double quote
quote(39). % `'`
quote(93). % `"'`

%% A digit is simply a number from 0-9
digit(C) :- 48 =< C, C =< 57.

%% "Lower" is either a letter or an underscore.
lower(95). % underscore.
lower(C) :- 97 =< C, C =< 122.

%% "Upper" is simply an uppercase number.
upper(C) :- 65 =< C, C =< 90.

%% These are all the symbols defined within knight.
symbol(C) :- member(C, "`!+-*/%^?&|;=<>").

