import { Value, TYPES } from './value';
import { ParseError } from './error';
import { Literal } from './literal';

/**
 * The string type within Knight, used to represent textual data.
 *
 * @see Value - For more information on why we don't simply use `String`.
 * @extends {Literal<string>}
 */
export class Str extends Literal<string> {
	/**
	 * Attempts to parse a `Str` from the `stream`.`
	 *
	 * @param { import('./stream').Stream } stream - The stream to parse from.
	 * @returns {Str|null} - The parsed `Str`, or `null` if the stream did not
	 *                       start with a string.
	 * @throws {ParseError} If no ending quote is found.
	 */
	static parse(stream) {
		// The modifier `/m` doesn't work in this case, so
		// `[\s\S]` is used to match _all_ characters, including `\n` and `\r\n`.
		const match = stream.match(/^(["'])([\s\S]*?)\1/, 2);

		// if we have a match, return early.
		if (match !== null) {
			return new Str(match);
		}

		// if we have a starting quote, it means the ending one didn't match.
		if (stream.match(/^['"]/)) {
			throw new ParseError(`Unterminated quote encountered: ${stream}`);
		}
	}

	/**
	 * Converts this class to a number, as per the knight spec.
	 *
	 * This does effectively what `parseInt` does, except it returns `0` instead
	 * of `NaN`.
	 * 
	 * @override
	 * @return {number} - The numeric representation of this class.
	 */
	toInt() {
		return parseInt(this.data, 10) || 0;
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
	 * @param {Value} rhs - The amount of times to repeat `this` for.
	 * @return {Str} - `This` repeated `rhs` times.
	 */
	mul(rhs) {
		return new Str(this.data.repeat(rhs.toInt()));
	}

	/**
	 * Returns a whether `this` is less than `rhs`, lexicographically 
	 *
	 * @param {Value} rhs - The value to be converted to a string and compared.
	 * @return {Str} - True if `this` is less than `rhs`.
	 */
	lth(rhs) {
		return this.data < rhs.toString();
	}

	/**
	 * Returns a whether `this` is less than `rhs`, lexicographically 
	 *
	 * @param {Value} rhs - The value to be converted to a string and compared.
	 * @return {Str} - True if `this` is less than `rhs`.
	 */
	gth(rhs) {
		return this.data > rhs.toString();
	}
}

TYPES.push(Str);
