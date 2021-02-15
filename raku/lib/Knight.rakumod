unit module Knight;

our &run = sub {
	require Knight::Parser '&parse-and-run';

	::('&parse-and-run').($^a)
};
