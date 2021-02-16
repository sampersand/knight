import { Value } from './Value.js';

export class Integer extends Value {
	#data;

	static parse(stream) {
		let match = stream.match(/^\d+/);

		return match ?? new Integer(Number(match[0]));
	}

	constructor(data) {
		super();

		if (!Number.isInteger(data)) {
			throw `Expected an integer, got ${typeof data}`;
		}

		this.#data = data;
	}

	toString() {
		return this.#data.toString();
	}

	toInteger() {
		return this.#data;
	}

	toBoolean() {
		return this.#data !== 0;
	}

	add(rhs) {
		return new Integer(this.#data + rhs.toInteger());
	}

	sub(rhs) {
		return new Integer(this.#data - rhs.toInteger());
	}

	mul(rhs) {
		return new Integer(this.#data * rhs.toInteger());
	}

	div(rhs) {
		rhs = rhs.toInteger();

		if (rhs === 0) {
			throw 'Cannot divide by zero';
		} else {
			return new Integer(Math.floor(this.#data / rhs));
		}
	}

	mod(rhs) {
		rhs = rhs.toInteger();

		if (rhs === 0) {
			throw 'Cannot modulo by zero';
		} else {
			return new Integer(this.#data % rhs);
		}
	}

	pow(rhs) {
		return new Integer(this.#data ** rhs.toInteger());
	}

	eql(rhs) {
		return rhs instanceof Integer && rhs.data === this;
	}

	cmp(rhs) {
		return sub(rhs); // comparison is the same as subtraction
	}
}
