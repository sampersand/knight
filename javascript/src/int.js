import { Value } from './value.js';

export class Int extends Value {
	#data;

	static parse(stream) {
		const match = stream.match(/^\d+/);

		return match && new Int(Number(match));
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

	toInt() {
		return this.#data;
	}

	toBool() {
		return this.#data !== 0;
	}

	add(rhs) {
		return new Int(this.#data + rhs.toInt());
	}

	sub(rhs) {
		return new Int(this.#data - rhs.toInt());
	}

	mul(rhs) {
		return new Int(this.#data * rhs.toInt());
	}

	div(rhs) {
		const rhsInt = rhs.toInt();

		if (rhsInt) {
			return new Int(Math.floor(this.#data / rhsInt));
		} else {
			throw new Error('Cannot divide by zero');
		}
	}

	mod(rhs) {
		const rhsInt = rhs.toInt();

		if (rhsInt) {
			return new Int(this.#data % rhsInt);
		} else {
			throw new Error('Cannot modulo by zero');
		}
	}

	pow(rhs) {
		return new Int(this.#data ** rhs.toInt());
	}

	eql(rhs) {
		return rhs instanceof Int && rhs.data === this;
	}

	cmp(rhs) {
		return sub(rhs); // comparison is the same as subtraction
	}
}

Value.TYPES.push(Int);
