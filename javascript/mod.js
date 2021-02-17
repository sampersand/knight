import { Value } from './value.js';
import { Bool } from './bool.js';
import { Ident } from './ident.js';
import { Int } from './int.js';
import { Nil } from './nil.js';
import { Str } from './str.js';
import { Func } from './func.js';
import { Stream } from './stream.js';

export function run(input) {
	let value = Value.parse(new Stream(input.toString()))

	if (value === null) {
		throw 'No value could be parsed!';
	} else {
		return value.run();
	}
}
