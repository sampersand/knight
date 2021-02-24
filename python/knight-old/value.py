import knight

class Value():
	@classmethod
	def parse(cls, stream):
		if not isinstance(stream, knight.Stream):
			stream = knight.Stream(stream)

		while stream.matches(r'(?:#.*?(\n|\Z)|\A[\s()\[\]{}:])*'):
			pass

		for subcls in [Number, String, Boolean, Identifier, Null, Ast]:
			if None != (value := subcls.parse(stream)):
				return value

	@classmethod
	def create(cls, data):
		if isinstance(data, Value):
			return data
		elif isinstance(data, str):
			return String(data)
		elif isinstance(data, bool):
			return Boolean(data)
		elif isinstance(data, int):
			return Number(data)
		elif data == None:
			return Null(None)
		else:
			raise TypeError(f'unknown value kind "{type(data)}"')

	def __init__(self, data):
		if type(self) == Value:
			raise RuntimeError('nope')

		self.data = data

	def __repr__(self):
		return f'{type(self).__name__}({self})'

	def run(self):
		return self

	def __str__(self):
		return str(self.run().data)

	def __int__(self):
		return int(self.run().data)

	def __bool__(self):
		return bool(self.run().data)

	def __add__(self, rhs):
		return Number(int(self) + int(rhs))

	def __sub__(self, rhs):
		return Number(int(self) - int(rhs))

	def __mul__(self, rhs):
		return Number(int(self) * int(rhs))

	def __div__(self, rhs):
		return Number(int(self) / int(rhs))

	def __mod__(self, rhs):
		return Number(int(self) % int(rhs))

	def __pow__(self, rhs):
		return Number(int(self) ** int(rhs))

	def __lt__(self, rhs):
		return int(self) < (int(rhs))

	def __eq__(self, rhs):
		return type(self) == type(rhs) and self.data == rhs.data

class Number(Value):
	@classmethod
	def parse(cls, stream):
		if match := stream.matches(r'\d+'):
			return Number(int(match))

class String(Value):
	@classmethod
	def parse(cls, stream):
		if match := stream.matches(r'(["\'])((?:.|\n)*?)(\1|\Z)'):
			if match[0] not in ['"', "'"] or match[0] != match[-1]:
				# note that the stream is still advanced...
				raise ArgumentError(f'unterminated string encountered: {match}')
			else:
				return String(match[1:-1])


	def __add__(self, rhs):
		return String(str(self) + str(rhs))

	def __mul__(self, rhs):
		return String(str(self) * int(rhs))

	def __lt__(self, rhs):
		return str(self) < str(rhs)

class Identifier(Value):
	@classmethod
	def parse(cls, stream):
		if match := stream.matches(r'[a-z_][a-z0-9_]*'):
			return Identifier(match)

	def run(self):
		return knight.ENVIRONMENT[self.data]

	def __repr__(self):
		return f'Identifier({self.data})'

class Ast(Value):
	@classmethod
	def parse(cls, stream):
		start = str(stream)[0]
		if func := knight.Function.known.get(start):
			stream.matches(r'[A-Z]+|.')
			return Ast(func, start, [Value.parse(stream) for _ in range(func.arity)])

	def __init__(self, func, name, args):
		self.func = func
		self.name = name
		self.args = args

	def __repr__(self):
		# return f'Value({repr(self.func)}{', '.join(repr(x) for x in self.args)})'
		return f'Function({self.name!r}{"".join(f", {x!r}" for x in self.args)})'

	def run(self):
		return self.func(*self.args)
