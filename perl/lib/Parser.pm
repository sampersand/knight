package Parser;
use strict;
use warnings;

use lib '.';

use Ast::String;
use Ast::Number;
use Ast::Boolean;
use Ast::Function;

use overload "bool" => sub { $_[0]->stream };

sub new($$) {
	my ($class, $stream) = @_;

	bless { stream => $stream }, $class;
}

sub next_token($$) {
	my 
	sub(/)
	return if $_
	return &next_token if s/\A([\s(){}\[\]:)]|#.*)//;
	shift;
	my $stream = shift;
}

# my $str = Ast::String->new("3");
# print 'a' . $$str;
my $stream = "123 abc 44";
print Ast::Number->parse(\$stream), "\n";
# print $stream
# print(Ast::Number->new("0") || Ast::Number->new("fl"));


1;
