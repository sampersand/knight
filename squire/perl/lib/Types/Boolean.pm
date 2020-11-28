package Boolean;

use Value;
use base qw(Value);

use strict;
use warnings;

sub not { Boolean->new(!shift->{value}); }

sub band(){ Boolean->new(shift->{value} & shift->to_number()->{value}); }
sub bor() { Boolean->new(shift->{value} | shift->to_number()->{value}); }
sub bxor(){ Boolean->new(shift->{value} ^ shift->to_number()->{value}); }

sub to_string { String->new(shift->{value} ? 'true' : 'false'); }
sub to_boolean{ shift }
sub to_number { Number->new(shift->{value} ? 1 : 0); }

sub cmp() { shift->to_number()->cmp(shift); }

1;
