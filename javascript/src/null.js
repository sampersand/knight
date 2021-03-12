import { TYPES } from './value.js';
import { RuntimeError } from './error.js';
import { Literal } from './literal.js';

/**
 * @typedef {import('./stream.js').Stream} Stream
 * @typedef {import('./value.js').Value} Value
 */

/**
 * The null type within Knight, used to represent the lack of a value.
 *
 * @see Value - For more information on why we don't simply use `null`.
 * @extends {Literal<null>}
 */
export class Null extends Literal {
	/**
	 * Attempts to parse a `Null` from the `stream`.
	 *
	 * @param {Stream} stream - The stream from which to parse.
	 * @return {Null|null} - The parsed `Null`, or `null` if the stream did not
	 *                       start with a `Null`.
	 */
	static parse(stream) {
		return stream.match(/^N[A-Z]*/) && new Null();
	}

	/** Creates a new `Null`. */
	constructor(){
		super(null);
	}

	/**
	 * Provides a debugging representation of this class.
	 *
	 * @return {string}
	 */
	dump() {
		return 'Null()';
	}

	/**
	 * Returns whether or not `this` is equal to `rhs`.
	 *
	 * This is simply a more efficient overload of the `eql` function, as we
	 * only need to check to see if `rhs` is `Null`.
	 *
	 * @override
	 * @param {Value} The value against which to compare.
	 * @return {boolean} Whether `rhs` is `Null`.
	 */
	eql(rhs) {
		return rhs instanceof Null;
	}

	/**
	 * Comparisons with `Null` are invalid, and this always fails.
	 *
	 * @param {Value} _rhs - Ignored.
	 * @throws {RuntimeError} This is always thrown.
	 */
	lth(_rhs) {
		throw new RuntimeError("Cannot compare Null.");
	}

	/**
	 * Comparisons with `Null` are invalid, and this always fails.
	 *
	 * @param {Value} _rhs - Ignored.
	 * @throws {RuntimeError} This is always thrown.
	 */
	gth(_rhs) {
		throw new RuntimeError("Cannot compare Null.");
	}
}

// Add the `Null` class to the list of known types, so it can be parsed.
TYPES.push(Null);
