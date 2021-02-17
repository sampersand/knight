import { Value } from './value.js';

export class Ident extends Value {
	static KNOWN = {};

	#ident;

	static parse(stream) {
		let match = stream.match(/^[a-z_][a-z0-9_]*/);

		return match && new Ident(match);
	}

	constructor(ident) {
		super();

		this.#ident = ident;
	}

	assign(value) {
		const result = value.run()

		Ident.KNOWN[this.#ident] = result;

		return result;
	}

	run() {
		const value = Ident.KNOWN[this.#ident];

		if (value === undefined) {
			throw `Unknown identifier '${this.#ident}'`;
		} else {
			return value;
		}
	}
}

Value.TYPES.push(Ident);
