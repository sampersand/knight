import { ParseError } from './error.js';
import { Value } from './value.js';
import { Stream } from './stream.js';

import { Bool } from './bool.js';
import { Ident } from './ident.js';
import { Int } from './int.js';
import { Null } from './null.js';
import { Str } from './str.js';
import { Func } from './func.js';

export * from './error.js';

export function run(input) {
	let value = Value.parse(new Stream(input.toString()));

	if (value !== null) {
		return value.run();
	} else {
		throw new ParseError('No value could be parsed!');
	}
}
