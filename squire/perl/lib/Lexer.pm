package Lexer;

use strict;
use warnings;

# use overload 'bool' => sub { shift->{stream} };
# use overload 'bool' => sub { shift }, '""' => sub { my $x = shift; $$x };

sub new {
	my $class = shift;
	# bless { stream => shift }, $class;
	bless \shift, $class;
}

sub peek {
	substr shift->{stream}, 0, 1
}

sub advance {
	shift->{stream} =~ s/\A.//;
}

sub next {
	my $self = shift;

	# while $self->{stream} =~ s/\A\s+// or 
		# return unless $self;
		# goto start if ;
	1
}

1;
