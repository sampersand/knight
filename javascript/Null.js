import { Value } from './Value.js';

export class Null extends Value {
	static parse(stream) {
		let match = stream.match(/^N[A-Z]*/);

		return match ?? new Null();
	}

	toString() {
		return 'null';
	}

	toInteger() {
		return 0;
	}

	toBoolean() {
		return false;
	}

	eql(rhs) {
		return rhs instanceof typeof this;
	}
}
