import { Value, TYPES } from './value.js';
import { RuntimeError } from './error.js';

export class Nil extends Value {
	static parse(stream) {
		return stream.match(/^N[A-Z]*/) && new Nil();
	}

	toString() {
		return 'null';
	}

	toInt() {
		return 0;
	}

	toBool() {
		return false;
	}

	dump() {
		return 'Null()';
	}

	eql(rhs) {
		return rhs instanceof Nil;
	}

	lth(_rhs) {
		throw new RuntimeError("Cannot compare Null.");
	}

	gth(_rhs) {
		throw new RuntimeError("Cannot compare Null.");
	}
}

TYPES.push(Nil);
