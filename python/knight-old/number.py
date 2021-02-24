from __future__ import annotations
from knight import Value, Stream, Literal
from typing import Union

class Number(Value):
	@classmethod
	def parse(cls, stream):
		if match := stream.matches(r'\d+'):
			return cls(int(match))

	def __init__(self, data):
		self.data = data

	def __repr__(self):
		return f'Number({self})'

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
