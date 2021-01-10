our %ENVIRONMENT = (qwert => 4);
use strict;
use warnings;

use Kn::Number;
use Kn::Value;

my $stream = $ARGV[0];
$stream = join("", <>);

Kn::Value->parse(\$stream)->run();
