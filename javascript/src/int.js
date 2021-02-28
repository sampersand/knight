import { TYPES } from './value.js';
import { Literal } from './literal.js';

/**
 * @typedef {import('./stream.js').Stream} Stream
 * @typedef {import('./value.js').Value} Value
 */

/**
 * The number type within Knight.
 *
 * As per the Knight specs, the only number type allowed are integers.
 *
 * @see Value - For more information on why we don't simply use `number`s.
 * @extends {Literal<number>}
 */
export class Int extends Literal {
	/**
	 * Attempts to parse an `Int` from the `stream`.
	 *
	 * @param {Stream} stream - The stream from which to parse.
	 * @return {Int|null} - The parsed `Int`, or `null` if the stream did not
	 *                      start with a `Int`.
	 */
	static parse(stream) {
		const match = stream.match(/^\d+/);

		return match && new Int(Number(match));
	}

	/**
	 * Provides a debugging representation of this class.
	 *
	 * @return {string}
	 */
	dump() {
		return `Number(${this})`;
	}

	/**
	 * Returns a new `Int` that is the result of adding `rhs` to `this`.
	 *
	 * @param {Value} rhs - The value to add to `this`.
	 * @return {Int} - The result of the addition.
	 */
	add(rhs) {
		return new Int(this._data + rhs.toNumber());
	}

	/**
	 * Returns a new `Int` that is the result of subtracting `rhs` from `this`.
	 *
	 * @param {Value} rhs - The value to subtract from `this`.
	 * @return {Int} - The result of the subtraction.
	 */
	sub(rhs) {
		return new Int(this._data - rhs.toNumber());
	}

	/**
	 * Returns a new `Int` that is the result of multiplying `this` by `rhs`.
	 *
	 * @param {Value} rhs - The value to multiply from `this`.
	 * @return {Int} - The result of the multiplication.
	 */
	mul(rhs) {
		return new Int(this._data * rhs.toNumber());
	}

	/**
	 * Returns a new `Int` that is the result of dividing `rhs` from `this`.
	 *
	 * Note that this will truncate the result of the division.
	 *
	 * @param {Value} rhs - The value to divide from `this`.
	 * @return {Int} - The result of the division.
	 * @throws {RuntimeError} - Thrown if `rhs` is zero.
	 */
	div(rhs) {
		const rhsInt = rhs.toNumber();

		if (rhsInt === 0) {
			throw new RuntimeError('Cannot divide by zero');
		} else {
			return new Int(Math.trunc(this._data / rhsInt));
		}
	}

	/**
	 * Returns a new `Int` that is the result of moduloing `this` by `this`.
	 *
	 * @param {Value} rhs - The base of the modulation.
	 * @return {Int} - The result of the modulo operation.
	 * @throws {RuntimeError} - Thrown if `rhs` is zero.
	 */
	mod(rhs) {
		const rhsInt = rhs.toNumber();

		if (rhsInt === 0) {
			throw new RuntimeError('Cannot modulo by zero');
		} else {
			return new Int(this._data % rhsInt);
		}
	}

	/**
	 * Returns a new `Int` that is the result of raising `this` to the `rhs`th
	 * power.
	 *
	 * Note that this will truncate the result of the exponentiation.
	 *
	 * @param {Value} rhs - The exponent
	 * @return {Int} - The result of the exponentiation.
	 */
	pow(rhs) {
		return new Int(Math.trunc(this._data ** rhs.toNumber()));
	}

	/**
	 * Returns whether `this` is numerically less than `rhs`.
	 *
	 * @param {Value} rhs - The value to convert to a number and compare against.
	 * @return {boolean} - Whether `this` is numerically less than `rhs`.
	 */
	lth(rhs) {
		return this._data < rhs.toNumber();
	}

	/**
	 * Returns whether `this` is numerically greater than `rhs`.
	 *
	 * @param {Value} rhs - The value to convert to a number and compare against.
	 * @return {boolean} - Whether `this` is numerically greater than `rhs`.
	 */
	gth(rhs) {
		return this._data > rhs.toNumber();
	}
}

// Add the `Int` class to the list of known types, so it can be parsed.
TYPES.push(Int);
