import { Value } from './value.js';

export class Bool extends Value {
	#data;

	static parse(stream) {
		const match = stream.match(/^([TF])[A-Z]*/, 1);

		return match && new Bool(match === 'T');
	}

	constructor(data) {
		super();

		if (typeof data !== 'boolean') {
			throw new Error(`Expected a boolean, got ${typeof data}`);
		}

		this.#data = data;
	}

	toString() {
		return this.#data.toString();
	}

	toInt() {
		return +this.#data;
	}

	toBool() {
		return this.#data;
	}

	eql(rhs) {
		return rhs instanceof Bool && this.#data === rhs.#data;
	}
}

Value.TYPES.push(Bool);
