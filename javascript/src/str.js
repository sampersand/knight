import { TYPES } from './value.js';
import { ParseError } from './error.js';
import { Literal } from './literal.js';

/**
 * @typedef {import('./stream.js').Stream} Stream
 * @typedef {import('./value.js').Value} Value
 */

/**
 * The string type within Knight, used to represent textual data.
 *
 * @see Value - For more information on why we don't simply use `string`s.
 * @extends {Literal<string>}
 */
export class Str extends Literal {
	/**
	 * Attempts to parse a `Str` from the `stream`.
	 *
	 * @param {Stream}  stream - The stream from which to parse.
	 * @returns {Str|null} - The parsed `Str`, or `null` if the stream did not
	 *                       start with a `Str`.
	 * @throws {ParseError} If a starting quote, but no ending quote, is parsed.
	 */
	static parse(stream) {
		// The modifier `/m` doesn't work in this case, so `[\s\S]` is used to
		// match _all_ characters, including `\n` and `\r\n`.
		const match = stream.match(/^(["'])([\s\S]*?)\1/, 2);

		// if we have a match, return early.
		if (match !== null) {
			return new Str(match);
		}

		// if we have a starting quote, it means the ending one didn't match.
		const first = stream.peek();
		if (first === "'" || first === '"') {
			throw new ParseError(`Unterminated quote encountered: ${stream}`);
		}
	}

	/**
	 * Converts this class to a number, as per the Knight spec.
	 *
	 * This does effectively what `parseInt` does, except it returns `0` instead
	 * of `NaN`.
	 *
	 * @override
	 * @return {number} - The numeric representation of this class.
	 */
	toNumber() {
		return parseInt(this._data, 10) || 0;
	}

	/**
	 * Provides a debugging representation of this class.
	 *
	 * @return {string}
	 */
	dump() {
		return `String(${this})`;
	}

	/**
	 * Returns a new `Str` with `this` concatenated with `rhs`.
	 *
	 * @param {Value} rhs - The value to append.
	 * @return {Str} - The concatenation of `this` and `rhs`.
	 */
	add(rhs) {
		return new Str(`${this}${rhs}`);
	}

	/**
	 * Returns a new `Str` with `this` repeated `rhs` times.
	 *
	 * @param {Value} rhs - The amount of times that `this` will be repeated.
	 * @return {Str} - `This` repeated `rhs` times.
	 */
	mul(rhs) {
		return new Str(this._data.repeat(rhs.toNumber()));
	}

	/**
	 * Returns a whether `this` is less than `rhs`, lexicographically.
	 *
	 * @param {Value} rhs - The value to be converted to a string and compared.
	 * @return {boolean} - True if `this` is less than `rhs`.
	 */
	lth(rhs) {
		return this._data < rhs.toString();
	}

	/**
	 * Returns a whether `this` is less than `rhs`, lexicographically.
	 *
	 * @param {Value} rhs - The value to be converted to a string and compared.
	 * @return {boolean} - True if `this` is less than `rhs`.
	 */
	gth(rhs) {
		return this._data > rhs.toString();
	}
}

// Add the `Str` class to the list of known types, so it can be parsed.
TYPES.push(Str);
