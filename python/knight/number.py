from __future__ import annotations
from knight import Value, Stream, Literal, RunError
from typing import Union
import re
import math

class Number(Literal[int]):
	"""
	The number class in Knight.

	As per the Knight specs, the only number type within Knight is
	integral numbers. As such, we use Python's builtin `int` class.
	"""
	REGEX: re.Pattern = re.compile(r'^\d+')

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, Number]:
		"""
		Parses a Number out from the stream.

		This returns `None` if the stream doesn't start with a digit.
		"""
		if match := stream.matches(Number.REGEX):
			return cls(int(match))
		else:
			return None

	def __add__(self, rhs: Value) -> Number:
		""" Converts `rhs` to an `int` and adds it to `self.` """
		return Number(int(self) + int(rhs))

	def __sub__(self, rhs: Value) -> Number:
		""" Converts `rhs` to an `int` and subtracts it from `self.` """
		return Number(int(self) - int(rhs))

	def __mul__(self, rhs: Value) -> Number:
		""" Converts `rhs` to an `int` and multiples it by it `self.` """
		return Number(int(self) * int(rhs))

	def __floordiv__(self, rhs: Value) -> Number:
		"""
		Converts `rhs` to an `int` and divides `self` by it, with the
		division operation conforming to the Knight specs.

		This will raise a `RunError` if `rhs` is zero.
		"""
		if rhs := int(rhs):
			return Number(math.trunc(int(self) / rhs))
		else:
			raise RunError('Cannot divide by zero!')

	def __mod__(self, rhs: Value) -> Number:
		"""
		Converts `rhs` to an `int` and modulos `self` by it, with the
		modulo operation conforming to the Knight specs.

		This will raise a `RunError` if `rhs` is zero.
		"""
		if rhs := int(rhs):
			return Number(int(self) % rhs)
		else:
			raise RunError('Cannot divide by zero!')

	def __pow__(self, rhs: Value) -> Number:
		"""
		Converts `rhs` to an `int` and exponentiates `self` by it, with
		the power ofoperation conforming to the Knight specs.
		"""
		return Number(math.trunc(int(self) ** int(rhs)))

	def __lt__(self, rhs: Value) -> bool:
		""" Checks to see if `self` is numerically less than `rhs`. """
		return int(self) < int(rhs)

	def __gt__(self, rhs: Value) -> bool:
		""" Checks to see if `self` is numerically greater than `rhs`. """
		return int(self) > int(rhs)
