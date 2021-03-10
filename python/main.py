#!/usr/bin/env python3

import knight
from sys import setrecursionlimit, argv

# This is needed to run FizzBuzz in Knight in Python.
setrecursionlimit(100000)

try:
	_, flag, program = argv
	assert flag in ['-e', '-f']
except:
	quit(f"usage: {argv[0]} (-e 'program' | -f file)")

if flag == '-f':
	with open(program) as f:
		program = f.read()

knight.run(program)
