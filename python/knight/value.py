from __future__ import annotations

from knight import Stream
from typing import Union

TYPES = []

class Value():
	"""
	The type in Knight that represents any representable entity.
	"""

	@staticmethod
	def parse(stream: Stream) -> Union[None, Value]:
		"""
		Parses a value out of the `stream`, or returns `None` if
		nothing can be parsed.
		"""
		stream.strip()

		for cls in TYPES:
			value = cls.parse(stream)

			if value is not None:
				return value

		return None

	def __init_subclass__(cls):
		""" Adds `cls` to the list of classes to parse. """
		super().__init_subclass__()

		TYPES.append(cls)

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
