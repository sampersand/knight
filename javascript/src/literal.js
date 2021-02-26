import { Value } from './value.js';

export class Literal extends Value {
	#data;

	constructor(data) {
		super();

		this.#data = data;
	}

	toString() {
		return String(this.#data);
	}

	toInt() {
		return Number(this.#data);
	}

	toBool() {
		console.log(this.#data, Boolean(this.#data));
		return Boolean(this.#data);
	}

	dump() {
		return `${this.constructor.name}(${this})`;
	}

	eql(rhs) {
		return rhs.constructor instanceof this.constructor && this.#data === rhs.#data;
	}
}
