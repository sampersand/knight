from __future__ import annotations
from knight import Value, Stream, ParseError, RunError, Identifier
from typing import Union, Dict, List
from random import randint

import re
import subprocess

class Function(Value):
	_KNOWN: Dict[str, callable] = {}
	REGEX: re.Pattern = re.compile(r'[A-Z]+|.')

	@staticmethod
	def parse(stream: Stream) -> Union[None, Function]:
		name = stream.peek()
		if name not in Function._KNOWN:
			return None

		func = Function._KNOWN[name]
		stream.matches(Function.REGEX)

		args = []
		for arg in range(func.__code__.co_argcount):
			value = Value.parse(stream)

			if value is None:
				raise ParseError(f'Missing argument {arg} for function {name}')

			args.append(value)

		return Function(func, name, args)

	def __init__(self, func: callable, name: str, args: list[Value]):
		self.func = func
		self.name = name
		self.args = args

	def run(self) -> Value:
		return self.func(*self.args)

Value.TYPES.append(Function)

def function(name=None):
	return lambda body: Function._KNOWN.__setitem__(
		name or body.__name__[0].upper(), body)

@function()
def prompt():
	return input()

@function()
def random():
	return randint(0, 0xff_ff_ff_ff)

@function()
def eval_(text):
	if value := Value.parse(Stream(str(text))):
		return value.run()
	else:
		raise ParseError('Nothing to parse.')

@function()
def block(blk):
	return blk

@function()
def call(blk):
	return blk.run().run()

@function('`')
def system(cmd):
	return subprocess.run(str(cmd), shell=True, capture_output=True) \
		.stdout.decode()

@function()
def quit_(code):
	quit(int(code))

@function('!')
def not_(arg):
	return not arg.run()

@function()
def length(arg):
	return len(str(arg))

@function()
def dump(arg):
	arg = arg.run()

	print(repr(arg))

	return arg

@function()
def output(arg):
	s = str(arg)

	if s[-1] == '\\':
		print(s[:-2], end='')
	else:
		print(s)

	return Null()

@function('+')
def add(lhs, rhs):
	return lhs.run() + rhs.run()

@function('-')
def sub(lhs, rhs):
	return lhs.run() - rhs.run()

@function('*')
def mul(lhs, rhs):
	return lhs.run() * rhs.run()

@function('/')
def div(lhs, rhs):
	return lhs.run() / rhs.run()

@function('%')
def mod(lhs, rhs):
	return lhs.run() % rhs.run()

@function('^')
def pow(lhs, rhs):
	return lhs.run() ** rhs.run()

@function('<')
def lth(lhs, rhs):
	return lhs.run() < rhs.run()

@function('>')
def gth(lhs, rhs):
	lhs = lhs.run()
	return rhs.run() > lhs

@function('?')
def eql(lhs, rhs):
	return lhs.run() == rhs.run()

@function('&')
def and_(lhs, rhs):
	return lhs.run() and rhs.run()

@function('|')
def or_(lhs, rhs):
	return lhs.run() or rhs.run()

@function(';')
def then(lhs, rhs):
	lhs.run()
	return rhs.run()

@function()
def while_(cond, body):
	while cond:
		body.run()

	return Null()

@function('=')
def assign(name, value):
	if not isinstance(name, Identifier):
		name = Identifier(str(name))

	value = value.run()
	name.assign(value)
	return value 

@function()
def if_(cond, iftrue, iffalse):
	return (iftrue if cond else iffalse).run()

@function()
def get(text, start, length):
	text = str(text)
	start = int(start)
	length = int(length)
	return text[start:start+length]

@function()
def set(text, start, length, repl):
	text = str(text)
	start = int(start)
	length = int(length)
	repl = str(repl)
	return text[:start] + repl + text[start+length:]
