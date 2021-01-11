#!/usr/bin/perl

use strict;
use warnings;

use lib 'lib';
use Kn;

# Prints a usage message and exists
sub usage {
	print STDERR "usage: $0 (-e program | -f file)\n";
	exit 1
}

my $sw = shift @ARGV;

usage unless $#ARGV == 0 && ($sw eq '-f' || $sw eq '-e');

if ($sw eq '-f') {
	Kn->run(join '', <>);
} elsif ($sw eq '-e') {
	Kn->run(shift @ARGV);
} else {
	usage;
}

