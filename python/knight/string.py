from __future__ import annotations
from knight import Value, Stream, Literal, ParseError
from typing import Union
import re

class String(Literal[bool]):
	"""
	The number class in Knight.

	As per the Knight specs, the only number type within Knight is
	integral numbers. As such, we use Python's builtin `int` class.
	"""

	BEGIN_REGEX: re.Pattern = re.compile(r'[\'\"]')
	SINGLE_REGEX: re.Pattern = re.compile(r"([^']*)'")
	DOUBLE_REGEX: re.Pattern = re.compile(r'([^"]*)"')
	INT_REGEX: re.Pattern = re.compile(r'^\s*[-+]?\d+')

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, String]:
		"""
		Parses a `String` from the `stream`, returning `None` if the
		nothing can be parsed.

		If a starting quote is matched and no ending quote is, then a
		`ParseError` will be raised.
		"""
		quote = stream.matches(String.BEGIN_REGEX)

		if not quote:
			return None

		regex = String.SINGLE_REGEX if quote == "'" else String.DOUBLE_REGEX
		body = stream.matches(regex, 1)

		if body is None:
			raise ParseError(f'unterminated string encountered: {stream}')
		else:
			return String(body)

	def __int__(self) -> int:
		"""
		Converts `self` to an integer, as per the Knight specs.

		Note that this is different from Python's conversions, as invalid
		numbers do not cause exceptions to be thrown, but rather handles
		them in a specific fashion. See the Knight specs for details.
		"""
		match = String.INT_REGEX.match(self.data)

		return int(match[0]) if match else 0

	def __add__(self, rhs: Value) -> String:
		""" Concatenates `self` and `rhs` """
		return String(f'{self}{rhs}')

	def __mul__(self, rhs: Value) -> int:
		""" Repeats `self` for `rhs` times """
		return String(str(self) * int(rhs))

	def __lt__(self, rhs: Value) -> bool:
		"""
		Checks to see if `self` is lexicographically less than `rhs`.
		"""
		return self.data < str(rhs)

	def __gt__(self, rhs: Value) -> bool:
		"""
		Checks to see if `self` is lexicographically less than `rhs`.
		"""
		return self.data > str(rhs)
