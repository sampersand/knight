class Environment():
	def __new__(cls):
		if 'INSTANCE' in dir(cls):
			return cls.INSTANCE
		else:
			cls.INSTANCE = super().__new__(cls)
			return cls.INSTANCE

	def __init__(self):
		self.vars = {}

	@staticmethod
	def __getitem__(self, item):
		return self.vars[item]

	def __setitem__(self, item, val):
		self.vars[item] = val

ENVIRONMENT = Environment()

