from __future__ import annotations
from knight import Value, Stream, Literal, RunError
from typing import Union
import re

class Null(Literal[None]):
	""" Used to represent the null class. """

	REGEX = re.compile(r'N[A-Z]*')

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, Null]:
		""" Parses `Null` if the stream starts with `N`. """
		if stream.matches(Null.REGEX):
			return cls()
		else:
			return None

	def __init__(self):
		"""
		Creates a new Null.

		Note that this is overloaded because `Literal` expects an argument
		for `data`, but `null` should be constructible without specifying
		the `data` field, so this does that for us.
		"""
		super().__init__(None)

	def __int__(self):
		""" Simply returns `0` """
		return 0

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
		return isinstance(rhs, Null)

	def __lt__(self, _: Value):
		""" Comparisons to Null are invalid. """
		raise RunError('cannot compare with Null.')

	def __gt__(self, _: Value):
		""" Comparisons to Null are invalid. """
		raise RunError('cannot compare with Null.')
