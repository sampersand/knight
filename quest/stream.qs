Kn = :0.?Kn.else(object);

Kn.Stream = object() {
	'()' = (class, stream) -> {
		stream.extend(class)
	};

	@text = {quit();_0.clone().becomes(Text).@text() };

	NOMATCH = :-1::object() {
		@bool = false.itself;
	};

	strip = self -> {
		self.match(/\A(?:[\s\[\]{}():]|\#.*\n)*/);
	};

	match = (self, regex, group) -> {
		match = regex.match(self).else(NOMATCH.return);

		self.replace(self[match.get(0).len(), -1]);

		match.get(group.or(0))
	}
};
