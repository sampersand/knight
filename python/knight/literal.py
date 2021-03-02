from __future__ import annotations
from knight import Value
from typing import TypeVar, Generic

T = TypeVar('T')

class Literal(Value, Generic[T]):
	"""
	A class used to represent a value that has a piece of data associated
	with it.

	Thi sis not meant to be initialized directly, and instead the
	subclasses of it should be used.
	"""

	data: T

	def __init__(self, data: T):
		super().__init__()
		self.data = data

	def run(self) -> Value:
		""" Running a Literal simply returns itself. """
		return self

	def __str__(self):
		""" Simply converts this class's `data` to a `str`. """
		return str(self.data)

	def __int__(self):
		""" Simply converts this class's `data` to an `int`. """
		return int(self.data)

	def __bool__(self):
		""" Simply converts this class's `data` to an `bool`. """
		return bool(self.data)

	def __repr__(self):
		""" Gets a debugging representation of this class. """
		return f'{type(self).__name__}({self})'

	def __eq__(self, rhs: Value):
		"""
		Returns whether `rhs` is of the _same_ class, 
		and their data is equivalent.
		"""
		return type(self) == type(rhs) and self.data == rhs.data
