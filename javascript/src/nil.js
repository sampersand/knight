import { Value, TYPES } from './value.js';

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

	eql(rhs) {
		return rhs instanceof Nil;
	}
}

TYPES.push(Nil);
