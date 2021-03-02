from __future__ import annotations
from knight import Value, Stream, Literal
from typing import Union
import re

class Boolean(Literal[bool]):
	""" Used to represent boolean types within Knight. """

	REGEX: re.Pattern = re.compile(r'([TF])[A-Z]*')
	TRUE: Boolean = None # `__new__` overrides this
	FALSE: Boolean = None # `__new__` overrides this

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, Boolean]:
		""" Parses a `Boolean` if the stream starts with `T` or `F`. """
		if match := stream.matches(Boolean.REGEX, 1):
			return cls(match == 'T')
		else:
			return None

	def __new__(cls, value: bool):
		""" Returns the TRUE or FALSE instance of this class. """
		if cls.TRUE is None: # initialize the instances if need
			cls.TRUE = super().__new__(cls)
			cls.FALSE = super().__new__(cls)

		return cls.TRUE if value else cls.FALSE

	def __str__(self):
		""" Simply returns `"true"` when true and `"false"` when false """
		return 'true' if self else 'false'

	def __eq__(self, rhs: Value):
		""" Booleans are only equal to themselves. """
		return self is rhs

	def __lt__(self, rhs: Value):
		return not self and rhs

	def __gt__(self, rhs: Value):
		return self and not rhs

Boolean(True) # ensure we initialize `Boolean`.
Value.TYPES.append(Boolean)
