import { Value, TYPES } from './value.js';
import { Literal } from './literal.js';

// The class in Knight that represents a boolean
export class Bool extends Literal {
	#data;

	static parse(stream) {
		const match = stream.match(/^([TF])[A-Z]*/, 1);

		return match && new Bool(match === 'T');
	}	

	constructor(data) {
		if (typeof data !== 'boolean') {
			throw new Error(`Expected a boolean, got ${typeof data}`);
		}
		super(data);
	}

	toString() {
		return this.#data.toString();
	}

	toInt() {
		return +this.#data;
	}

	// toBool() {
	// 	return this.#data;
	// }

	dump() {
		return `Boolean(${this})`;
	}

	eql(rhs) {
		return rhs instanceof Bool && this.#data === rhs.#data;
	}

	lth(rhs) {
		return !this.#data && rhs.toBool();
	}

	gth(rhs) {
		return this.#data && !rhs.toBool();
	}
}

TYPES.push(Bool);

