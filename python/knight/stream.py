import re
from typing import Union 

class Stream():
	""" The class used when parsing data. """

	def __init__(self, source: str):
		""" Creates a new `Stream` with the given source. """
		self.source = source

	def __str__(self):
		""" Returns the remainder of the stream. """
		return self.source

	def __bool__(self):
		""" Returns whether the stream is empty. """
		return bool(self.source)

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
