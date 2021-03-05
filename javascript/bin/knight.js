#!/usr/bin/env node

import { KnightError, run } from '../src/knight.js';
import { readFileSync } from 'fs';

const argv = process.argv;

if (argv.length !== 4 || (argv[2] !== '-e' && argv[2] !== '-f')) {
	console.error(`usage: ${argv[1]} (-e 'program' | -f file)`);
	process.exit(1);
}

try {
	run(argv[2] == '-e' ? argv[3] : readFileSync(argv[3]));
} catch (error) {
	if (error instanceof KnightError) {
		console.error("Fatal Error:", error.message);
		process.exit(1);
	} else {
		throw error;
	}
}
