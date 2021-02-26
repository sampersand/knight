import { Value, TYPES } from './value.js';
import { RuntimeError } from './error.js';

const ENVIRONMENT = {};

export class Ident extends Value {
	#ident;

	static parse(stream) {
		const match = stream.match(/^[a-z_][a-z0-9_]*/);

		return match && new Ident(match);
	}

	constructor(ident) {
		super();

		if (typeof ident !== 'string') {
			throw new Error(`Expected a string, got ${typeof ident}`);
		}

		this.#ident = ident;
	}

	assign(value) {
		ENVIRONMENT[this.#ident] = value;
	}

	run() {
		const value = ENVIRONMENT[this.#ident];

		if (value) {
			return value;
		} else {
			throw new RuntimeError(`Unknown identifier '${this.#ident}'`);
		}
	}

	dump() {
		return `Identifier(${this.#ident})`;
	}
}

TYPES.push(Ident);
