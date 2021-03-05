from __future__ import annotations
from knight import Value, Stream, Literal, RunError
from typing import Union
import re
import math

class Number(Literal[int]):
	REGEX: re.Pattern = re.compile(r'^\d+')

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, Number]:
		if match := stream.matches(Number.REGEX):
			return cls(int(match))
		else:
			return None

	def __add__(self, rhs: Value) -> Number:
		return Number(int(self) + int(rhs))

	def __sub__(self, rhs: Value) -> Number:
		return Number(int(self) - int(rhs))

	def __mul__(self, rhs: Value) -> Number:
		return Number(int(self) * int(rhs))

	def __floordiv__(self, rhs: Value) -> Number:
		if rhs := int(rhs):
			return Number(math.trunc(int(self) / rhs))
		else:
			raise RunError('Cannot divide by zero!')

	def __mod__(self, rhs: Value) -> Number:
		if rhs := int(rhs):
			return Number(int(self) % rhs)
		else:
			raise RunError('Cannot divide by zero!')

	def __pow__(self, rhs: Value) -> Number:
		return Number(math.trunc(int(self) ** int(rhs)))

	def __lt__(self, rhs: Value) -> bool:
		return int(self) < int(rhs)

	def __gt__(self, rhs: Value) -> bool:
		return int(self) > int(rhs)

Value.TYPES.append(Number)
