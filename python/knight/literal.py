from __future__ import annotations
from knight import Value
from typing import TypeVar, Generic

T = TypeVar('T')

class Literal(Value, Generic[T], parse=False):
	"""
	A class used to represent a value that has a piece of data associated
	with it.

	Thi sis not meant to be initialized directly, and instead the
	subclasses of it should be used.
	"""

	data: T

	def __init__(self, data: T):
		""" Creates a new `Literal` instance with the given data. """
		super().__init__()
		self.data = data

	def run(self) -> Value:
		""" Running a Literal simply returns itself. """
		return self

	def __str__(self) -> str:
		""" Simply converts this class's `data` to a `str`. """
		return str(self.data)

	def __int__(self) -> int:
		""" Simply converts this class's `data` to an `int`. """
		return int(self.data)

	def __bool__(self) -> bool:
		""" Simply converts this class's `data` to an `bool`. """
		return bool(self.data)

	def __repr__(self) -> str:
		""" Gets a debugging representation of this class. """
		return f'{type(self).__name__}({self})'

	def __eq__(self, rhs: Value) -> bool:
		"""
		Returns whether `rhs` is of the _same_ class, 
		and their data is equivalent.
		"""
		return type(self) == type(rhs) and self.data == rhs.data
