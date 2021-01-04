import knight

class Value():
	@classmethod
	def parse(cls, stream):
		if not isinstance(stream, knight.Stream):
			stream = knight.Stream(stream)

		while stream.matches(r'(?:#.*?(\n|\Z)|\A[\s()\[\]{}:])*'):
			pass

		for subcls in [Number, Text, Boolean, Identifier, Null, Ast]:
			if None != (value := subcls.parse(stream)):
				return value

	@classmethod
	def create(cls, data):
		if isinstance(data, Value):
			return data
		elif isinstance(data, str):
			return Text(data)
		elif isinstance(data, bool):
			return Boolean(data)
		elif isinstance(data, int):
			return Number(data)
		elif data == None:
			return Null(None)
		else:
			raise TypeError(f"unknown value kind '{type(data)}'")

	def __init__(self, data):
		if type(self) == Value:
			raise RuntimeError("nope")
		self.data = data

	def __repr__(self):
		return f"Value({repr(self.data)})"

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

class Text(Value):
	@classmethod
	def parse(cls, stream):
		if match := stream.matches(r'(["\'])((?:.|\n)*?)(\1|\Z)'):
			if match[0] not in ['"', '\''] or match[0] != match[-1]:
				# note that the stream is still advanced...
				raise ArgumentError("unterminated string encountered: " + match)
			else:
				return Text(match[1:-1])


	def __add__(self, rhs):
		return Text(str(self) + str(rhs))

	def __mul__(self, rhs):
		return Text(str(self) * int(rhs))

	def __lt__(self, rhs):
		return str(self) < str(rhs)

class Boolean(Value):
	@classmethod
	def parse(cls, stream):
		if match := stream.matches(r'[TF][A-Z]*'):
			return Boolean(match[0] == 'T')

	def __str__(self):
		return "true" if self.data else "false"

class Null(Value):
	@classmethod
	def parse(cls, stream):
		if match := stream.matches(r'N[A-Z]*'):
			return Null(None)

	def __str__(self):
		return "null"

class Identifier(Value):
	@classmethod
	def parse(cls, stream):
		if match := stream.matches(r'[a-z_][a-z0-9_]*'):
			return Identifier(match)

	def run(self):
		return knight.ENVIRONMENT[self.data]

class Ast(Value):
	@classmethod
	def parse(cls, stream):
		if func := knight.Function.known.get(str(stream)[0]):
			stream.matches(r'[A-Z]+|.')
			return Ast(func, [Value.parse(stream) for _ in range(func.arity)])

	def __init__(self, func, args):
		self.func = func
		self.args = args

	def __repr__(self):
		return f"Value({repr(self.func)}, {repr(self.args)})"

	def run(self):
		return self.func(*self.args)
