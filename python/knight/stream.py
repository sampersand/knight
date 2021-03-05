import re
from typing import Union 

class Stream():
	""" The class used when parsing data. """
	WHITESPACE: re.Pattern = re.compile(r'([\s(){}\[\]:]+|\#[^\n]*)+')

	def __init__(self, source: str):
		""" Creates a new `Stream` with the given source. """
		self.source = source

	def __str__(self) -> str:
		""" Returns the remainder of the stream. """
		return self.source

	def __bool__(self) -> bool:
		""" Returns whether the stream is empty. """
		return bool(self.source)

	def strip(self):
		""" Removes all leading whitespace and quotes """
		self.matches(Stream.WHITESPACE)

	def peek(self) -> Union[None, str]:
		""" Returns the first character of the stream """
		return self.source[0] if self.source else None

	def matches(self, rxp: re.Pattern, index: int = 0) -> Union[None, str]:
		"""
		Checks to see if the start of the stream matches `rxp`.

		If the stream doesn't match, `None` is returned. Otherwise, the
		stream is updated, and the `index`th group is returned. (The
		default value of `0` means the entire matched regex is returned.)
		"""
		match = rxp.match(self.source)

		if match is None:
			return None

		string = self.source[:match.end()]
		self.source = self.source[match.end():]
		return match[index]
