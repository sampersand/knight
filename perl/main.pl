use strict;
use warnings;

use lib 'lib';
use Kn;

use constant USAGE => "usage: $0 (-e program | -f file)";

my $sw = shift @ARGV;

die USAGE unless $#ARGV == 0 && ($sw eq '-f' || $sw eq '-e');

if ($sw eq '-f') {
	Kn->run(join '', <>);
} elsif ($sw eq '-e') {
	Kn->run(shift @ARGV);
} else {
	die USAGE;
}
