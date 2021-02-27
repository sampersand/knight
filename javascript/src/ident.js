import { Value, TYPES } from './value.js';
import { RuntimeError } from './error.js';

/**
 * The list of all known identifiers.
 *
 * @type {Object.<string, Value>}
 */
const ENVIRONMENT = {};

/**
 * The identifier class within Knight.
 *
 * As per the specs, all identifiers are global. As such, there is no scoping
 * whatsoever, and a single `ENVIRONMENT` is used.
 */
export class Ident extends Value {
	/** @type {string} */
	#ident;

	/**
	 * Attempts to parse an `Ident` from the `stream`.`
	 *
	 * @param { import('./stream.js').Stream } stream - The stream to parse from.
	 * @returns {Ident|null} - The parsed identifier, or `null` if the stream did
	 *                         not start with an identifier.
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
	 * Note that this will actually `run` the `value`---this is because it allows
	 * for a much easier implementation of `assign`, where `= (+ "a" x) ...` will
	 * only evaluate `value` after it evaluates the `Ident`.
	 *
	 * Any previously associated `Value` is simply discarded.
	 * 
	 * @param {Value} value - The value to associate with this identifier's name.
	 * @return {Value} - The result of running `value`.
	 */
	assign(value) {
		value = value.run();

		ENVIRONMENT[this.#ident] = value;

		return value;
	}

	/**
	 * Executes this identifier, returning its last assigned value.
	 *
	 * @return {Value} - The last value associated with this identifier's name.
	 * @throws [RuntimeError] - Thrown if this identifier was never assigned to.
	 */
	run() {
		const value = ENVIRONMENT[this.#ident];

		if (value) {
			return value;
		} else {
			throw new RuntimeError(`Unknown identifier '${this.#ident}'`);
		}
	}
}

TYPES.push(Ident);
