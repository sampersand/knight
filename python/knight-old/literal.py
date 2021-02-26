from __future__ import annotations
from knight import Value, Stream
from typing import Union

class Literal(Value):
	""" Used to represent a value that has a piece of data associated with it. """

	def __init__(self, data):
		self.data = data

	def run(self) -> Value:
		return self

	def __str__(self) -> str:
		return str(self.data)

	def __int__(self) -> int:
		return int(self.data)

	def __bool__(self) -> bool:
		return bool(self.data)

	def __repr__(self) -> str:
		return f'{type(self).__name__}({repr(self.data)})'

	def __eq__(self, rhs: Value) -> bool:
		return type(self) == type(rhs) and self.data == rhs.data
