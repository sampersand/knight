import { Value, TYPES } from './value.js';
import { RuntimeError } from './error.js';

export class Null extends Value {
	static parse(stream) {
		return stream.match(/^N[A-Z]*/) && new Null();
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
