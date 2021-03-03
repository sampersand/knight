from __future__ import annotations
from knight import Value, Stream, Literal
from typing import Union

class Boolean(Literal):
	""" The Boolean type in Knight --- used to represent truth values. """
	def __init__(self, data: bool):
		super().__init__(data)

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, Boolean]:
		""" Attempts to parse a `Boolean """
		if match := stream.matches(r'([TF])[A-Z]*', 1):
			return cls(match == 'T')

	def __str__(self):
		""" Returns `"true"` when true and `"false"` when false. """
		return 'true' if self.data else 'false'

	def __lt__(self, rhs: Value):
		""" Checks to see if `self` is falsey and `rhs` is truthy. """
		return self.data < bool(rhs)

	def __gt__(self, rhs: Value):
		""" Checks to see if `self` is truthy and `rhs` is falsey. """
		return self.data > bool(rhs)
