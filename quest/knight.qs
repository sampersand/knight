Kn = object();

Io::File('classes.qs').read().eval();
Io::File('stream.qs').read().eval();

run = what -> {
	stream = Kn::Stream(what.@text());
	parsed = Kn::Value.parse(stream);
	(parsed == stream::NOMATCH).then(abort << "invalid stream!");
	parsed.run()
};


run(Io::File('../examples/fizzbuzz.kn').read());
##__EOF__##
stream = Kn.Stream("; = a + 3 4 : * a 4");
stream = Kn.Stream("; = a 45 : + 'abc' a");
stream = Kn.Stream("O ` 'ls -al ../*'");

Kn::Value.parse(stream).run();
##__EOF__##
print(stream);
quit(0);
print(stream.match(/\AF/));
print(stream.match(/\AF/));
print(stream);
quit();
