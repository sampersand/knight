from __future__ import annotations
from knight import Value, Stream, Literal, RunError
from typing import Union
import re

class Number(Literal[int]):
	REGEX: re.Pattern = re.compile(r'^\d+')

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, Number]:
		if match := stream.matches(Number.REGEX):
			return cls(int(match))
		else:
			return None

	def __add__(self, rhs):
		return Number(int(self) + int(rhs))

	def __sub__(self, rhs):
		return Number(int(self) - int(rhs))

	def __mul__(self, rhs):
		return Number(int(self) * int(rhs))

	def __div__(self, rhs):
		if rhs := int(rhs):
			return Number(int(self) / rhs)
		else:
			raise RunError('Cannot divide by zero!')

	def __mod__(self, rhs):
		if rhs := int(rhs):
			return Number(int(self) % rhs)
		else:
			raise RunError('Cannot divide by zero!')

	def __pow__(self, rhs):
		return Number(int(self) ** int(rhs))

	def __lt__(self, rhs):
		return int(self) < int(rhs)

	def __gt__(self, rhs):
		return int(self) > int(rhs)

Value.TYPES.append(Number)
