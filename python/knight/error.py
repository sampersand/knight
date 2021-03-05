class Error(Exception):
	""" The parent error class to all Knight errors."""

class ParseError(Error):
	""" A problem occurred whilst parsing Knight code. """

class RunError(Error):
	""" A problem occurred whilst running Knight code. """
