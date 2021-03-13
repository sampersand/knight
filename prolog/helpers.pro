%% Whitespace is any of `\t\n\v\f\r`.
whitespace(C) :- member(C, " \t\n\v\f\r").

%% A Quote is either a single or double quote
quote(C) :- member(C, "\"\'").

%% A digit is simply a number from 0-9
digit(C) :- member(C, "0123456789").

%% An underscore, used for parsing part of `upper`.
underscore(95).

%% "Lower" is either a letter or an underscore.
lower(C) :- member(C, "abcdefghijklmnopqrstuvwxyz"); underscore(C).

%% "Upper" is simply an uppercase number.
upper(C) :- member(C, "ABCDEFGHIJKLMNOPQRSTUVWXYZ").
upper_under(C) :- upper(C); underscore(C).

%% These are all the symbols defined within knight.
symbol(C) :- member(C, "`!+-*/%^?&|;=<>").
