from __future__ import annotations
from knight import Value, Stream, ParseError, RunError, \
                   Identifier, Boolean, Null, String, Number
from typing import Union, Dict, List
from random import randint

import re
import subprocess

_FUNCS: Dict[str, callable] = {}

class Function(Value):
	""" Used to represent functions and their arguments within Knight. """

	REGEX: re.Pattern = re.compile(r'[A-Z]+|.')

	@staticmethod
	def parse(stream: Stream) -> Union[None, Function]:
		"""
		Parses a `Function` from the stream, returning `None` if the
		stream didn't start with a function character.

		This will both parse the function name, and its arguments. If not
		all the arguments could be parsed, a `ParseError` is raised.
		"""
		name = stream.peek()
		if name not in _FUNCS:
			return None

		func = _FUNCS[name]
		stream.matches(Function.REGEX)

		args = []
		for arg in range(func.__code__.co_argcount):
			value = Value.parse(stream)

			if value is None:
				raise ParseError(f'Missing argument {arg} for function {name}')

			args.append(value)

		return Function(func, name, args)

	def __init__(self, func: callable, name: str, args: list[Value]):
		"""
		Creates a new function that'll execute `func` with `args`.

		Note that the `name` is only used for `__repr__`.
		"""
		self.func = func
		self.name = name
		self.args = args

	def run(self) -> Value:
		return self.func(*self.args)

	def __repr__(self) -> str:
		return f'Function({self.name}, {self.args})'

def register(name: Union[None, str] = None) -> callable:
	"""
	Used to register a new function with the given name.

	If no name is supplied, it will use the upper-case version of the
	first letter of the function's name.
	"""
	return lambda fn: _FUNCS.__setitem__(name or fn.__name__[0].upper(), fn)

@register()
def prompt() -> String:
	""" Reads a single line from stdin. """
	return String(input())

@register()
def random() -> Number:
	""" Returns a random number from 0 through 0xffff_ffff. """
	return Number(randint(0, 0xffff_ffff))

@register()
def eval_(text: Value) -> Value:
	""" Evaluates `text` as Knight code, returning its result. """
	value = Value.parse(Stream(str(text)))

	if value is None:
		raise ParseError('Nothing to parse.')
	else:
		return value.run()

@register()
def block(blk: Value) -> Value:
	""" Simply returns its argument, unevaluated. """
	return blk

@register()
def call(blk: Value) -> Value:
	""" Executes the return value of a `block`. """
	return blk.run().run()

@register('`')
def system(cmd: Value) -> String:
	""" Runs `cmd` in a shell, returning its stdout.  """
	proc = subprocess.run(str(cmd), shell=True, capture_output=True)

	return String(proc.stdout.decode())

@register()
def quit_(code: Value):
	""" Quits with the given status code. """
	quit(int(code))

@register('!')
def not_(arg: Value) -> Boolean:
	""" Negates its argument. """
	return Boolean(not arg)

@register()
def length(arg: Value) -> Number:
	""" Gets the length of its argument. """
	return Number(len(str(arg)))

@register()
def dump(arg: Value) -> Value:
	""" Dumps a debug representation of `arg` and returns `arg`. """
	arg = arg.run()

	print(repr(arg), end='')

	return arg

@register()
def output(arg: Value) -> Null:
	"""
	Prints `arg` to stdout with a trailing newline.

	If `arg` ends with a `\\`, the newline is omitted and the slash is 
	removed.
	"""
	s = str(arg)

	if s[-1] == '\\':
		print(s[:-2], end='')
	else:
		print(s)

	return Null()

@register('+')
def add(lhs: Value, rhs: Value) -> Value:
	""" Adds `rhs` to `lhs`. """
	return lhs.run() + rhs.run()

@register('-')
def sub(lhs: Value, rhs: Value) -> Value:
	""" Subtracts `rhs` from `lhs`. """
	return lhs.run() - rhs.run()

@register('*')
def mul(lhs: Value, rhs: Value) -> Value:
	""" Multiplies `lhs` by `rhs`. """
	return lhs.run() * rhs.run()

@register('/')
def div(lhs: Value, rhs: Value) -> Value:
	""" Divides `lhs` by `rhs`. """
	return lhs.run() // rhs.run()

@register('%')
def mod(lhs: Value, rhs: Value) -> Value:
	""" Modulos `lhs` by `rhs`. """
	return lhs.run() % rhs.run()

@register('^')
def pow(lhs: Value, rhs: Value) -> Value:
	""" Exponentiates `lhs` by `rhs`. """
	return lhs.run() ** rhs.run()

@register('<')
def lth(lhs: Value, rhs: Value) -> Boolean:
	""" Checks to see if `lhs` is less than `rhs`. """
	return Boolean(lhs.run() < rhs.run())

@register('>')
def gth(lhs: Value, rhs: Value) -> Boolean:
	""" Checks to see if `lhs` is greater than `rhs`. """
	return Boolean(lhs.run() > rhs.run())

@register('?')
def eql(lhs: Value, rhs: Value) -> Boolean:
	""" Checks to see if `lhs` is equal to `rhs`. """
	return Boolean(lhs.run() == rhs.run())

@register('&')
def and_(lhs: Value, rhs: Value) -> Value:
	""" Returns `lhs` if its falsey, otherwise `rhs`. """
	return lhs.run() and rhs.run()

@register('|')
def or_(lhs: Value, rhs: Value) -> Value:
	""" Returns `lhs` if its truthy, otherwise `rhs`. """
	return lhs.run() or rhs.run()

@register(';')
def then(lhs: Value, rhs: Value) -> Value:
	""" Simply executes `lhs`, then `rhs`, then returns `rhs`. """
	lhs.run()
	return rhs.run()

@register()
def while_(cond: Value, body: Value) -> Null:
	""" Executes `body` while `cond` is truthy. """
	while cond:
		body.run()

	return Null()

@register('=')
def assign(name: Value, value: Value) -> Value:
	"""
	Assigns `value` to `name`, converting `name` to an `Identifier` if
	it isn't already.

	Returns `value`.
	"""
	if not isinstance(name, Identifier):
		name = Identifier(str(name))

	value = value.run()
	name.assign(value)
	return value 

@register()
def if_(cond: Value, iftrue: Value, iffalse) -> Value:
	""" Executes and returns `iftrue` or `iffalse` based on `cond`. """
	return (iftrue if cond else iffalse).run()

@register()
def get(text: Value, start: Value, amnt: Value) -> String:
	""" Fetches the specified substring from `text`. """
	text = str(text)
	start = int(start)
	amnt = int(amnt)
	return String(text[start:start+amnt])

@register()
def substitute(text: Value, start: Value, amnt: Value, repl: Value) -> String:
	""" Returns a new string with the specified substring replaced. """
	text = str(text)
	start = int(start)
	amnt = int(amnt)
	repl = str(repl)
	return String(text[:start] + repl + text[start+amnt:])
