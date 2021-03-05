from __future__ import annotations
from knight import Value, Stream, Literal
from typing import Union
import re

class Boolean(Literal[bool]):
	""" Used to represent boolean types within Knight. """

	REGEX: re.Pattern = re.compile(r'([TF])[A-Z]*')

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, Boolean]:
		""" Parses a `Boolean` if the stream starts with `T` or `F`. """
		if match := stream.matches(Boolean.REGEX, 1):
			return cls(match == 'T')
		else:
			return None

	def __str__(self):
		""" Returns `"true"` when true and `"false"` when false. """
		return 'true' if self else 'false'

	def __lt__(self, rhs: Value):
		""" Checks to see if `self` is falsey and `rhs` is truthy. """
		return not self and rhs

	def __gt__(self, rhs: Value):
		""" Checks to see if `self` is truthy and `rhs` is falsey. """
		return self and not rhs
