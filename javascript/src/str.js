import { Value, TYPES } from './value.js';
import { ParseError } from './error.js';

export class Str extends Value {
	#data;

	static parse(stream) {
		// The modifier `/m` doesn't work in this case, so
		// `[\s\S]` is used to match _all_ characters, including `\n` and `\r\n`.
		const match = stream.match(/^(["'])([\s\S]*?)\1/, 2);

		if (match !== null) {
			return new Str(match);
		}

		if (stream.match(/^['"]/)) {
			throw new ParseError(`Unterminated quote encountered: ${stream}`);
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

	dump() {
		return `String(${this})`;
	}

	add(rhs) {
		return new Str(`${this.#data}${rhs}`);
	}

	mul(rhs) {
		return new Str(this.#data.repeat(rhs.toInt()));
	}

	eql(rhs) {
		return rhs instanceof Str && this.#data === rhs.#data;
	}

	lth(rhs) {
		return this.#data < rhs.toString();
	}

	gth(rhs) {
		return this.#data > rhs.toString();
	}
}

TYPES.push(Str);
