import { Value, TYPES } from './value.js';
import { RuntimeError } from './error.js';
import { Literal } from './literal.js';

export class Null extends Literal {
	static parse(stream) {
		return stream.match(/^N[A-Z]*/) && new Null();
	}

	constructor(){
		super(null);
	}

	toString() {
		return 'null';
	}

	dump() {
		return 'Null()';
	}

	eql(rhs) {
		return rhs instanceof Null;
	}

	lth(_rhs) {
		throw new RuntimeError("Cannot compare Null.");
	}

	gth(_rhs) {
		throw new RuntimeError("Cannot compare Null.");
	}
}

TYPES.push(Null);
