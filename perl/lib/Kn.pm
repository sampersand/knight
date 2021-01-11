package Kn;
use strict;
use warnings;

use Kn::Value;

# Runs the given argument as a knight program.
sub run {
	my $str = "$_[1]";
	Kn::Value->parse(\$str)->run();
}

1;
