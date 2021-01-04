from knight import *

import sys

# lol
sys.setrecursionlimit(10000)

if len(sys.argv) != 3 or sys.argv[1] not in ['-e', '-f']:
	quit(f"usage: {sys.argv[0]} [-e 'program'] [-f 'file']")

if sys.argv[1] == '-e':
	program = sys.argv[2]
else:
	with open(sys.argv[2]) as f:
		program = f.read()

Value.parse(program).run()
