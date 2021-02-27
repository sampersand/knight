import { ParseError } from './error';
import { Value } from './value';
import { Stream } from './stream';

import { Bool } from './bool';
import { Ident } from './ident';
import { Int } from './int';
import { Null } from './null';
import { Str } from './str';
import { Func } from './func';

export * from './error';

export function run(input) {
	let value = Value.parse(new Stream(input.toString()));

	if (value !== null) {
		return value.run();
	} else {
		throw new ParseError('No value could be parsed!');
	}
}
