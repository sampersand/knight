unit module Knight;

#| Converts the input to a string, then executes it.
our &run = sub {
	require Knight::Parser '&parse-and-run';

	::('&parse-and-run').($^a)
};
