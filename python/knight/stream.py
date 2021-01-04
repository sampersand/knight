import re

class Stream():
	def __init__(self, stream):
		self.stream = stream

	def __str__(self):
		return self.stream

	def __bool__(self):
		return bool(self.stream)

	def matches(self, rxp):
		match = re.match(rxp, self.stream)
		if match:
			string = self.stream[:match.end()]
			self.stream = self.stream[match.end():]
			return string
