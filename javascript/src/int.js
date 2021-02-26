import { Value, TYPES } from './value.js';

export class Int extends Value {
	#data;

	static parse(stream) {
		const match = stream.match(/^\d+/);

		return match && new Int(Number(match));
	}

	constructor(data) {
		super();

		if (!Number.isInteger(data)) {
			throw new Error(`Expected an integer, got ${typeof data}`);
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

	dump() {
		return `Number(${this})`;
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
			return new Int(Math.trunc(this.#data / rhsInt));
		} else {
			throw new RuntimeError('Cannot divide by zero');
		}
	}

	mod(rhs) {
		const rhsInt = rhs.toInt();

		if (rhsInt) {
			return new Int(this.#data % rhsInt);
		} else {
			throw new RuntimeError('Cannot modulo by zero');
		}
	}

	pow(rhs) {
		return new Int(Math.trunc(this.#data ** rhs.toInt()));
	}

	eql(rhs) {
		return rhs instanceof Int && this.#data == rhs.#data;
	}

	lth(rhs) {
		return this.#data < rhs.toInt();
	}

	gth(rhs) {
		return this.#data > rhs.toInt();
	}
}

TYPES.push(Int);
