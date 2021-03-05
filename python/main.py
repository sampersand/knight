#!/usr/bin/env python3

import knight
from sys import setrecursionlimit, argv

setrecursionlimit(1000000)

if len(argv) != 3 or argv[1] not in ['-e', '-f']:
	quit(f"usage: {argv[0]} (-e 'program' | -f file)")

if argv[1] == '-e':
	program = argv[2]
else:
	with open(argv[2]) as f:
		program = f.read()

knight.run(program)
