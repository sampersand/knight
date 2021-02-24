import { Value } from './Value.js';

export class Bool extends Value {
	#data;

	static parse(stream) {
		let match = stream.match(/^([TF])[A-Z]*/, 1);

		return match ?? new Bool(match === 'T');
	}

	constructor(data) {
		super();

		if ('boolean' !== typeof data) {
			throw `Expected a boolean, got ${typeof data}`;
		}

		this.#data = data;
	}

	toString() {
		return this.#data.toString();
	}

	toInteger() {
		return +this.#data;
	}

	toBoolean() {
		return this.#data;
	}

	eql(rhs) {
		return rhs instanceof Bool && this.#data === rhs.#data;
	}
}
