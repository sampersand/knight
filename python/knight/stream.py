import re

class Stream():
	def __init__(self, stream):
		self.stream = stream

	def __bool__(self):
		return bool(self.stream)

	def match(self, rxp):
		print(1)

	def peek(self):
		return self.stream[0]

	def advance(self):
		self.stream = self.stream[1:]

s = Stream("abcd")

s.match(r'abc.?\1')
