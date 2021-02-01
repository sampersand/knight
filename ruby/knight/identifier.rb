module Kn
	# An identifier within Knight
	class Identifier
		# The list of known variables.
		#
		# One of the few valid reasons to use a class variable:
		# - All instances need to access it
		# - It's not meant to be directly accessible by anyone else
		# - (most importantly) We don't have any subclasses.
		@@env = {}

		# Parse an identifier out of the steram.
		#
		# Identifiers are defined as starting with `a-z` or `_`, and then any
		# amount of `a-z`, `_`, or digits.
		def self.parse(stream) = stream.slice!(/\A[a-z_][a-z0-9_]*/)&.then { new _1 }

		# Initializes the identifier.
		def initialize(data) = @data = data

		# You assign an identifier's value by calling `assign`
		def assign(value) = @@env[@data] = value

		# Executing an identifier fetches it from the list of identifiers.
		def run = @@env.fetch(@data) { raise RunError, "unknown variable '#@data'." }
	end
end
