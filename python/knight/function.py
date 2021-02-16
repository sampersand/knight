import knight
from random import randint
import subprocess

class Function():
	known = {}

	def __init__(self, func):
		self.func = func

	@property
	def arity(self):
		return self.func.__code__.co_argcount

	def __call__(self, *args):
		return knight.Value.create(self.func(*args))

def function(name=None):
	return lambda body: Function.known.__setitem__(name or body.__name__[0].upper(), Function(body))

@function()
def true():
	return True

@function()
def false():
	return False

@function()
def null():
	return None

@function()
def prompt():
	return input()

@function()
def random():
	return randint(0, 0xff_ff_ff_ff) # return random between 0 and 4294967295

@function()
def eval_(text):
	return knight.Value.parse(str(text.run())).run() # evaluate as knight code

@function()
def block(blk):
	return blk

@function()
def call(blk):
	return blk.run().run()

@function('`')
def system(arg):
	text = str(arg.run())
	return subprocess.run(text, shell=True, capture_output=True).stdout.decode()

@function()
def quit_(code):
	quit(int(code.run()))

@function('!')
def not_(arg):
	return not arg.run() # not

@function()
def length(arg):
	return len(str(arg.run())) # return lenght

@function()
def output(arg): # print
	ret = arg.run()
	s = str(ret)

	if s[-1] == '\\':
		print(s[:-2], end='')
	else:
		print(s) 

	return ret

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
	return rhs.run() < lhs

@function('?')
def eql(lhs, rhs):
	return lhs.run() == rhs.run()

@function('&')
def and_(lhs, rhs):
	lhs = lhs.run()
	return rhs.run() if lhs else lhs

@function('|')
def or_(lhs, rhs):
	lhs = lhs.run()
	return lhs if lhs else rhs.run()

@function(';')
def then(lhs, rhs):
	lhs.run()
	return rhs.run()

@function()
def while_(cond, body):
	ret = None

	while cond.run():
		ret = body.run()

	return ret

@function('=') # assignment
def assign(name, value):
	name = name.data if isinstance(name, knight.Identifier) else str(name.run())
	value = value.run()
	knight.ENVIRONMENT[name] = value
	return value 

@function() # if statments
def if_(cond, iftrue, iffalse):
	return (iftrue if cond.run() else iffalse).run()

@function()
def get(text, start, length):
	text = str(text.run())
	start = int(start.run())
	length = int(length.run())
	return text[start:start+length]

@function()
def set(text, start, length, repl):
	text = str(text.run())
	start = int(start.run())
	length = int(length.run())
	repl = str(repl.run())
	return text[:start] + repl + text[start+length:]
