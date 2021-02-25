import re
from  typing import Union 

class Stream():
	""" The class used when parsing data """
	def __init__(self, source: str):
		""" Creates a new `Stream` with the given source. """
		self.source = source

	def __str__(self):
		""" Returns the remainder of the stream. """
		return self.source

	def __bool__(self):
		""" Returns whether the stream is empty. """
		return bool(self.source)

	def strip(self):
		""" Removes all leading whitespace and quotes """
		self.matches(r'([\s(){}\[\]:]+|\#[^\n]*)+')

	def prepend(self, value: str):
		"""
		Adds text back to the stream.

		This is only used by `String` to restore the stream back to its previous state
		when raising an error due to a missing closing quote.
		"""
		self.source = value + self.source

	def matches(self, rxp: str, index: int = 0) -> Union[None, str]:
		"""
		Checks to see if the start of the stream matches `rxp`.

		If the stream doesn't match, `None` is returned. Otherwise, the stream is updated, and the `index`th group is
		returned. (The default value of `0` means the entire matched regex is returned.)
		"""

		if match := re.match(rxp, self.source):
			string = self.source[:match.end()]
			self.source = self.source[match.end():]
			return string[index]
