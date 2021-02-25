from __future__ import annotations
from knight import Value, Stream, Literal
from typing import Union
import re

class String(Literal[bool]):
	BEGIN_REGEX: re.Pattern = re.compile(r'[\'\"]')
	SINGLE_REGEX: re.Pattern = re.compile(r"([^']*)'")
	DOUBLE_REGEX: re.Pattern = re.compile(r'([^"]*)"')
	INT_REGEX: re.Pattern = re.compile(r'^\s*\d+')

	@classmethod
	def parse(cls, stream: Stream) -> Union[None, String]:
		if not (quote := stream.matches(String.BEGIN_REGEX)):
			return

		regex = String.SINGLE_REGEX if quote == "'" else String.DOUBLE_REGEX

		if body := stream.matches(regex, 1):
			return String(body)
		else:
			raise ValueError(f'unterminated string encountered: {stream}')

	def __int__(self):
		if match := String.INT_REGEX.match(self.data):
			return int(match[0])
		else:
			return 0

	def __add__(self, rhs: Value):
		return String(str(self) + str(rhs))

	def __mul__(self, rhs: Value):
		return String(str(self) * int(rhs))

	def __lt__(self, rhs: Value):
		return self.data < str(rhs)

	def __gt__(self, rhs: Value):
		return self.data > str(rhs)
