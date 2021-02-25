from __future__ import annotations
from knight import Value, Stream, Literal, RunError
from typing import Union
import re

class Null(Literal[None]):
	""" Used to represent the null class. """

	REGEX = re.compile(r'N[A-Z]*')
	INSTANCE = None # `__new__` overrides this 

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, Null]:
		""" Parses `Null` if the stream starts with `N`. """
		if stream.matches(REGEX):
			return cls()
		else:
			return None

	def __new__(cls):
		""" Returns the instance of this class. """
		if cls.INSTANCE is None: # this is run once.
			cls.INSTANCE = super().__new__(cls)

		return cls.INSTANCE

	def __init__(self):
		super().__init__(None)

	def __str__(self):
		""" Simply returns `"null"` """
		return 'null'

	def __repr__(self):
		""" Gets a debugging representation of this class. """
		return 'Null()'

	def __eq__(self, rhs: Value):
		"""
		Null is only equal to itself, and as we have one `INSTANCE`, we
		can use `is`.
		"""
		return self is rhs

	def __lt__(self, _: Value):
		raise RunError('cannot compare Null.')

	def __gt__(self, _: Value):
		raise RunError('cannot compare Null.')

Null() # ensure we initialize `Null`.
