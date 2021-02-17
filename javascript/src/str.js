import { Value } from './value.js';

export class Str extends Value {
	#data;

	static parse(stream) {
		const match = stream.match(/^(["'])(.*?)\1/, 2);

		if (match) {
			return new Str(match);
		}

		if (stream.match(/^['"]/)) {
			throw `Unterminated quote encountered: ${stream}`;
		}
	}

	constructor(data) {
		super();

		if (typeof data !== 'string') {
			throw new Error(`Expected a string, got ${typeof data}`);
		}

		this.#data = data;
	}

	toString() {
		return this.#data;
	}

	toInt() {
		return parseInt(this.#data, 10) || 0;
	}

	toBool() {
		return this.#data !== "";
	}

	add(rhs) {
		return new Str(`${this.#data}${rhs}`);
	}

	mul(rhs) {
		return new Str(this.#data.repeat(rhs.toInt()));
	}

	eql(rhs) {
		return rhs instanceof Str && rhs.data === this.#data;
	}

	cmp(rhs) {
		return this.data.localeCompare(rhs.toString());
	}
}

Value.TYPES.push(Str);
