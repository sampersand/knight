#!/usr/bin/env perl

use strict;
use warnings;

# Not really 'perlish', as you'd generally install Perl modules globally. But, since this isn't really intended to be
# installed globally, I've dynamically loaded the the path to `/lib`.
use File::Basename qw/dirname basename/;
use lib dirname(__FILE__) . '/../lib';

use Kn;

# Prints a usage message and exists.
sub usage {
	print STDERR "usage: " . basename(__FILE__) . " (-e program | -f file)\n";
	exit 1;
}

my $switch = shift @ARGV;

usage unless $#ARGV == 0 && ($switch eq '-f' || $switch eq '-e');

if ($switch eq '-f') {
	Kn->run(join '', <>);
} elsif ($switch eq '-e') {
	Kn->run(shift @ARGV);
} else {
	usage;
}

