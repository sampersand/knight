import { Value } from './Value.js';

export class Str extends Value {
	#data;

	static parse(stream) {
		let match = stream.match(/^(["'])(.*?)\1/, 2);

		switch (true) {
			case match !== null:
				return new Str(match);
			case stream.match(/^['"]/) !== null:
				throw `Unterminated quote encountered: ${stream}`;
			default:
				return null;		
		}
	}


	constructor(data) {
		super();

		if ('string' !== typeof data) {
			throw `Expected a string, got ${typeof data}`;
		}

		this.#data = data;
	}

	toString() {
		return this.#data;
	}

	toInteger() {
		return parseInt(this.#data, 10) || 0;
	}

	toBoolean() {
		return this.#data !== "";
	}

	add(rhs) {
		return new Str(`${this.#data}${rhs}`);
	}

	mul(rhs) {
		return new Str(this.#data.repeat(rhs.toInteger()));
	}

	eql(rhs) {
		return rhs instanceof typeof(this) && rhs.data === this.#data;
	}

	cmp(rhs) {
		return this.data.localeCompare(rhs.toString());
	}
}
