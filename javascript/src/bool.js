import { Value, TYPES } from './value.js';
import { Literal } from './literal.js';

/**
 * The boolean type within Knight, used to represent truthfulness.
 *
 * @see Value - For more information on why we don't simply use `true`/`false`.
 * @extends {Literal<boolean>}
 */
export class Bool extends Literal {
	/**
	 * Attempts to parse a `Bool` from the `stream`.`
	 *
	 * @param { import('./stream.js').Stream } stream - The stream to parse from.
	 * @returns {Bool|null} - The parsed boolean, or `null` if the stream did not
	 *                        start with a boolean.
	 */
	static parse(stream) {
		const match = stream.match(/^([TF])[A-Z]*/, 1);

		return match && new Bool(match === 'T');
	}

	/**
	 * Provides a debugging representation of this class.
	 *
	 * @return {string}
	 */
	dump() {
		return `Boolean(${this})`;
	}

	/**
	 * Checks to see if `this` is less than `rhs`.
	 *
	 * This will only return true if `this.toBool()` is false and `rhs.toBool()`
	 * is true.
	 *
	 * @return {boolean} - Whether or not `this` is less than `rhs`.
	 */
	lth(rhs) {
		return !this._data && rhs.toBool();
	}

	/**
	 * Checks to see if `this` is greater than `rhs`.
	 *
	 * This will only return true if `this.toBool()` is true and `rhs.toBool()`
	 * is false.
	 *
	 * @return {boolean} - Whether or not `this` is greater than `rhs`.
	 */
	gth(rhs) {
		return this._data && !rhs.toBool();
	}
}

// Add the `Bool` class to the list of known types, so it can be parsed.
TYPES.push(Bool);
