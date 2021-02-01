Kn = :0.?Kn.else(object);

Kn.Value = object(Comparable) {
	'()' = (class, value) -> {
		value.run = class::run;
	 	value.inherit(class)
	};

	_v = :-1::true;

	run = ~$itself;

	assign = (self, value) -> {
		Kn::Variable(self.@text()).assign(value);
	};


	from = value -> {
		parents = value.__parents__.map({ _0.__id__ }).@list();
		parent = :0;

		if (parents.include?(Text.__id__)) {
			Kn::String(value).return(parent);
		};

		if (parents.include?(Number.__id__)) {
			Kn::Number(value).return(parent);
		};

		if (parents.include?(RustClosure.__id__)) {
			Kn::Number(value).return(parent);
		};

		if ((value == true).or(value == false)) {
			Kn::Boolean(value).return(parent);
		};

		if (null == value) {
			Kn::Null(value).return(parent);
		};

		abort("Unknown value given: " + value.inspect());
	};

	parse = (_class, stream) -> {
		parent = :0;

		stream.strip();

		[Kn::Variable, Kn::Number, Kn::String, Kn::Boolean, Kn::Null, Kn::Function].each {
			parsed = _0.parse(stream);

			unless (parsed == stream::NOMATCH) {
				parsed.return(parent);
			}
		};

		stream::NOMATCH
	}
};

Kn.Null = object(Kn::Value) {
	'<=>' = (_, _) -> {
		abort("Cannot compare Null.");
	};

	parse = (class, stream) -> {
		stream
			.match(/\AN[A-Z]*/)
			.then(class)
	};
};

Kn.Boolean = object(Kn::Value) {
	'<=>' = (self, rhs) -> {
		self.@num() <=> rhs.@bool().@num()
	};

	parse = (class, stream) -> {
		stream
			.match(/\A([TF])[A-Z]*/, 1)
			.then_into { class(_0 == 'T') }
	};
};

Kn.Number = object(Kn::Value) {
	parse = (class, stream) -> {
		stream
			.match(/\A\d+/)
			.then_into(~$@num)
			.then_into(class)
	};
};

Kn.String = object(Kn::Value) {
	@num = self -> {
		self.strip().gsub(/\D.*/, "").@num()
	};

	'*' = (self, amnt) -> {
		1.upto(amnt)
			.map(self.itself)
			.@list()
			.join()
	};

	parse = (class, stream) -> {
		quote = stream.match(/\A['"]/).else(stream::NOMATCH.return);
		regex = ifl(quote == "'", /\A([^']*)'/, /\A([^"]*)"/);

		match = stream.match(regex, 1);

		if (match == stream::NOMATCH) {
			abort("unmatched quote closing quote!: " + quote + stream);
		};

		class(match)
	};
};


Kn.Variable = object(Kn::Value) {
	ENVIRONMENT = :-1::object();
	'()' = (class, value) -> { value.becomes(class) };

	@text = self -> { self.run().@text() };
	@num = self -> { self.run().@num() };
	@bool = self -> { self.run().@bool() };

	run = self -> {
		ENVIRONMENT.__has_attr__(self).else {
			abort("unknown variable '" + self + "'");
		};

		print(self);

		ENVIRONMENT.(self)
	};

	assign = (self, torun) -> {
		ENVIRONMENT.(self) = torun.run()
	};

	parse = (class, stream) -> {
		stream
			.match(/\A[a-z_][a-z_0-9]*/)
			.then_into(class)
	};
};


Kn.Function = object(Kn::Value) {
	FUNCTIONS = :-1::object(:-1::Kernel) {
		 P  = [] -> { prompt() };
		 R  = [] -> { rand(0, 0xffff_ffff).round() };

		 E  = code -> { Knight.run(code) };
		 B  = block -> { block };
		 C  = block -> { block.run().run() };
		'`' = code -> { system('sh', '-c', code) };
		 Q  = status -> { quit(status) };
		'!' = value -> { !value };
		 L = string -> { string.@text().len() };
		 O = input -> {
		 	result = input.run();
		 	string = result.@text();

		 	if('\\' == string[-1], {
		 		printn(string[0, -2])
		 	}, {
		 		print(string)
		 	});

		 	result
		};

		'+' = (lhs, rhs) -> { lhs.run() + rhs.run() };
		'-' = (lhs, rhs) -> { lhs.run() - rhs.run() };
		'*' = (lhs, rhs) -> { lhs.run() * rhs.run() };
		'/' = (lhs, rhs) -> { lhs.run() / (rhs.@num().else(abort << "Cannot divide by zero")) };
		'%' = (lhs, rhs) -> { lhs.run() % (rhs.@num().else(abort << "Cannot modulo by zero")) };
		'^' = (lhs, rhs) -> { lhs.run() **rhs.run() };
		'<' = (lhs, rhs) -> { lhs.run() < rhs.run() };
		'>' = (lhs, rhs) -> { lhs.run() > rhs.run() };
		'?' = (lhs, rhs) -> { lhs.run() == rhs.run() };
		'&' = (lhs, rhs) -> { lhs.run().then(rhs.run) };
		'|' = (lhs, rhs) -> { lhs.run().else(rhs.run) };
		';' = (lhs, rhs) -> { lhs.run(); rhs.run() };
		'=' = (lhs, rhs) -> { lhs.assign(rhs) };
		 W  = (cond, body) -> {
			ret = null;
			while (cond) {
				:1.ret = body
			};
			ret
		};

		 I  = (cond, iftrue, iffalse) -> { if(cond.run(), iftrue.run, iffalse.run) };
		 G  = (string, start, length) -> {
		 	string.@text()[(start=start.@num()), start + length]
		 };
		 S  = (string, start, length, replacement) -> {
		 	abort("todo");
		 	#string.@text()[(start=start.@num()), start + length]
		};
	};

	parse = (class, stream) -> {
		match = stream.match(/\A([A-Z]+|.)/).else(return)[0];

		func = FUNCTIONS.?(match).else { abort("unknown function: '" + match + "'") };

		args = 1.upto(func.args.len())
			.map({
				Kn::Value.parse(stream)
					.tap {
						if (_0 == stream::NOMATCH) {
							abort("missing a parameter");
						}
					}
			}).@list();
		class(func.apply << args)
	};

	run = self -> { Kn::Value::from(self()) };
};


