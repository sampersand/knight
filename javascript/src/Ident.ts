import { Value, TYPES } from './value';
import { RuntimeError } from './error';
import { Stream } from './stream';

const ENVIRONMENT = {};

export class Ident extends Value {
	private ident: string;

	static parse(stream: Stream): (null | Ident) {
		const match = stream.match(/^[a-z_][a-z0-9_]*/);

		return match && new Ident(match);
	}

	constructor(ident: string) {
		super();

		if (typeof ident !== 'string') {
			throw new Error(`Expected a string, got ${typeof ident}`);
		}

		this.ident = ident;
	}

	assign(value: Value): Value {
		value = value.run();
		ENVIRONMENT[this.ident] = value;
		return value;
	}

	run(): Value {
		const value = ENVIRONMENT[this.ident];

		if (value) {
			return value;
		} else {
			throw new RuntimeError(`Unknown identifier '${this.ident}'`);
		}
	}

	dump(): string {
		return `Identifier(${this.ident})`;
	}
}

TYPES.push(Ident);
