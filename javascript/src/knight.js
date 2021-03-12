import { ParseError } from './error.js';
import { Value } from './value.js';
import { Stream } from './stream.js';

import {   } from './bool.js';
import { Ident } from './ident.js';
import { Int } from './int.js';
import { Null } from './null.js';
import { Str } from './str.js';
import { Func } from './func.js';

// Only KnightError is exported by default.
export { KnightError } from './error.js';

/**
 * Parses and executes the input as Knight code.
 *
 * @param {string} input - The string to parse and execute.
 * @return {Value} - The result of executing the code.
 */
export function run(input) {
	let value = Value.parse(new Stream(input.toString()));

	if (value === null) {
		throw new ParseError('No value could be parsed!');
	} else {
		return value.run();
	}
}
