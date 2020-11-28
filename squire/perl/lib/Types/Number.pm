package Number;

use Value;
use base qw(Value);

use strict;
use warnings;

sub add() { Number->new(shift->{value} + shift->to_number()->{value}); }
sub sub() { Number->new(shift->{value} - shift->to_number()->{value}); }
sub mul() { Number->new(shift->{value} * shift->to_number()->{value}); }
sub div() { Number->new(shift->{value} / shift->to_number()->{value}); }
sub mod() { Number->new(shift->{value} % shift->to_number()->{value}); }
sub pow() { Number->new(shift->{value} **shift->to_number()->{value}); }

sub band(){ Number->new(shift->{value} & shift->to_number()->{value}); }
sub bor() { Number->new(shift->{value} | shift->to_number()->{value}); }
sub bxor(){ Number->new(shift->{value} ^ shift->to_number()->{value}); }
sub shl() { Number->new(shift->{value}<<(shift->to_number()->{value}));}
sub shr() { Number->new(shift->{value} >>shift->to_number()->{value}); }
sub cmp() { Number->new(shift->{value}<=>shift->to_number()->{value}); }

sub to_number { shift; }
sub to_boolean{ Boolean->new(shift->{value} != 0); }

sub parse {
	$_[1] =~ s/\A\d+\b//p and Number->new(${^MATCH})
}

1;
