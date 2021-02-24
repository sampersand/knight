from __future__ import annotations
from knight import Literal, Stream
from typing import Union

class Null(Literal):
	""" The null type within Knight. """
	def __init__(self):
		""" You construct Null without giving it data, but it internally uses None. """
		super().__init__(None)

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, Null]:
		""" Parses `Null` if the stream starts with `N`. """
		if match := stream.matches(r'N[A-Z]*'):
			return cls()

	def __str__(self) -> str:
		""" Simply returns 'null' """
		return 'null'

	def __repr__(self) -> str:
		""" Used for debugging; returns `Null()`. """
		return 'Null()'
