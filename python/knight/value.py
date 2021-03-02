from __future__ import annotations

from knight import Stream
from typing import Union

class Value():
	"""
	The type in Knight that represents any representable entity.
	"""
	TYPES = []

	@staticmethod
	def parse(stream: Stream) -> Union[None, Value]:
		""" Parses a value out of the `stream`, or returns `None` if nothing can be parsed. """
		stream.strip()

		for cls in Value.TYPES:
			value = cls.parse(stream)

			if value is not None:
				return value

		return None

	def run(self) -> Value:
		""" Return the result of running this value. """
		raise NotImplemented

	def __int__(self):
		""" Converts this class to an integer. """
		return int(self.run())

	def __str__(self):
		""" Converts this class to a string. """
		return str(self.run())

	def __bool__(self):
		""" Converts this class to a boolean. """
		return bool(self.run())

	def __lt__(self, rhs: Value):
		""" Checks to see if `self` is less than `rhs`. """
		return self.run() < rhs.run()

	def __gt__(self, rhs: Value):
		""" Checks to see if `self` is greater than `rhs`. """
		return self.run() > rhs.run()
