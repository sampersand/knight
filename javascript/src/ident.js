import { Value, TYPES } from './value.js';
import { RuntimeError } from './error.js';

/**
 * @typedef {import('./stream.js').Stream} Stream
 */

/**
 * The list of all known identifiers.
 *
 * @private
 * @type {Object.<string, Value>}
 */
const ENVIRONMENT = {};

/**
 * The identifier class within Knight.
 *
 * As per the specs, all identifiers are global. As such, there is no scoping
 * whatsoever: A single `ENVIRONMENT` is used.
 */
export class Ident extends Value {
	/** @type {string} */
	#ident;

	/**
	 * Attempts to parse an `Ident` from the `stream`.
	 *
	 * @param {Stream} stream - The stream with which to parse.
	 * @return {Ident|null} - The parsed `Ident`, or `null` if the stream did not
	 *                        start with an `Ident`.
	 */
	static parse(stream) {
		const match = stream.match(/^[a-z_][a-z0-9_]*/);

		return match && new Ident(match);
	}

	/**
	 * Creates a new `Ident` with the given name.
	 *
	 * @param {string} ident - The name of this identifier.
	 */
	constructor(ident) {
		super();

		this.#ident = ident;
	}

	/**
	 * Provides a debugging representation of this class.
	 *
	 * @return {string}
	 */
	dump() {
		return `Identifier(${this.#ident})`;
	}

	/**
	 * Associates `value` with this class.
	 *
	 * Any previously associated `Value` is simply discarded.
	 *
	 * @param {Value} value - The value to associate with this identifier's name.
	 */
	assign(value) {
		ENVIRONMENT[this.#ident] = value;
	}

	/**
	 * Executes this identifier, returning its last assigned value.
	 *
	 * @return {Value} - The last value associated with this identifier's name.
	 * @throws {RuntimeError} - Thrown if this identifier was never assigned to.
	 */
	run() {
		const value = ENVIRONMENT[this.#ident];

		if (value === undefined) {
			throw new RuntimeError(`Unknown identifier '${this.#ident}'`);
		} else {
			return value;
		}
	}
}

// Add the `Ident` class to the list of known types, so it can be parsed.
TYPES.push(Ident);
