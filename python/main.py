from knight import *

x="abcd"
import re
r = re.compile(r'.')
x = r.sub('!', x)
print(x)
# print(dir())
print(Function.known['S'](
	Value('abcde'),
	Value(1),
	Value(0),
	Value("q")
))

# def foo(x): pass

# print(dir(foo))

# # print(dir())
