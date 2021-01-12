#n

# this program isn't done at all, :(

s/^$/foobar/;h;s/.*/baz/;H


s/.*/1299/
s/.*/299/
s/.*/300/

:inc
q
# add a leading `0` if we need a new digit
s/([^0-9]|$)(9+)$/\10\2/
/[0-8]9*$/ {
	s//&/
	G
	h
	s/^[^]*.([0-9]+)\n.*/\1/
	y/0123456789/1234567890/
	G
	s/^([0-9]+)\n([^]*).[0-9]+(.*)/\2\1\3/
	h
	s/[^]*.\n//
	x
	s/\n.*//
}
l
