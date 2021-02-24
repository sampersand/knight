import { Value } from './Value.js';

class UnknownIdentifierException extends Error {
	constructor(ident) {
		super(`Unknown identifier '${ident}'`);
	}
}

export class Identifier extends Value {
	static KNOWN = {};

	#ident;

	constructor(ident) {
		super();

		this.#ident = ident;
	}

	assign(value) {
		value = value.run()

		Identifier.KNOWN[this.#ident] = value;

		return value;
	}

	run() {
		if (Identifier.KNOWN.hasOwnProperty(this.#ident)) {
			return Identifier.KNOWN[this.#ident];
		} else {
			throw new UnknownIdentifierException(this.#ident);
		}
	}
}
