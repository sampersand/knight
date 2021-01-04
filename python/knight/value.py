from knight import ENVIRONMENT
class Value():
	@classmethod
	def parse(cls, stream):
		return NotImplemented

# 		def self.parse(stream)
# 			nil until stream.slice!(/\A(?:#.*?(\n|\z)|\A[\s()\[\]{}:])*/).empty?

# 			[Number, Text, Boolean, Identifier, Null, Ast]
# 				.lazy
# 				.filter_map { _1.parse(stream) }
# 				.first or abort "unknown character start #{stream[0].inspect}"
# 		end
	def __new__(cls, data):
		if isinstance(data, Value):
			data = data.data

		if cls != Value:
			return super().__new__(cls)
		elif isinstance(data, str):
			return Text(data)
		elif isinstance(data, bool):
			return Boolean(data)
		elif isinstance(data, int):
			return Number(data)
		elif data == None:
			return Null()
		else:
			raise TypeError(f"unknown value kind {type(data)}")

	def __init__(self, data):
		super().__init__()
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
		return Value(int(self) + int(rhs))

	def __sub__(self, rhs):
		return Value(int(self) - int(rhs))

	def __mul__(self, rhs):
		return Value(int(self) * int(rhs))

	def __div__(self, rhs):
		return Value(int(self) / int(rhs))

	def __mod__(self, rhs):
		return Value(int(self) % int(rhs))

	def __pow__(self, rhs):
		return Value(int(self) ** int(rhs))

	def __cmp__(self, rhs):
		return int(self).__cmp__(int(rhs))

	def __eq__(self, rhs):
		return type(self) == type(rhs) and self.data == rhs.data

class Number(Value):
	@classmethod
	def parse(cls, stream):
		return NotImplemented

class Text(Value):
	@classmethod
	def parse(cls, stream):
		return NotImplemented

	def __add__(self, rhs):
		return Value(str(self) + str(rhs))

	def __mul__(self, rhs):
		return Value(str(self) * int(rhs))

	def __cmp__(self, rhs):
		return str(self).__cmp__(str(rhs))

class Boolean(Value):
	@classmethod
	def parse(cls, stream):
		return NotImplemented

	def __str__(self):
		return "true" if self.data else "false"

class Null(Value):
	@classmethod
	def parse(cls, stream):
		return NotImplemented

	def __new__(cls):
		return super().__new__(cls, None)

	def __init__(self):
		super().__init__(None)

	def __str__(self):
		return "null"

class Identifier(Value):
	@classmethod
	def parse(cls, stream):
		return NotImplemented

	def run(self):
		return ENVIRONMENT[self.data]

class Ast(Value):
	@classmethod
	def parse(cls, stream):
		return NotImplemented

	def __init__(self, func, args):
		self.func = func
		self.args = args

	def run(self):
		self.func(*self.args)
