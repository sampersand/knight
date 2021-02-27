import { Value, TYPES } from './value.js';
import { Literal } from './literal.js';

/**
 * The number type within Knight.
 *
 * As per the Knight specs, the only number type allowed are integers.
 *
 * @see Value - For more information on why we don't simply use numbers.
 * @extends {Literal<number>}
 */
export class Int extends Literal {
	/**
	 * Attempts to parse an `Int` from the `stream`.`
	 *
	 * @param { import('./stream.js').Stream } stream - The stream to parse from.
	 * @returns {Int|null} - The parsed integer, or `null` if the stream did not
	 *                       start with a integer.
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
	 * Returns whether `this` is less than `rhs`, when `rhs` is converted to an
	 * `Int`.
	 *
	 * @param {Value} rhs - The value to compare to
	 * @return {boolean} - Whether `this` is numerically less than `rhs`.
	 */
	lth(rhs) {
		return this._data < rhs.toNumber();
	}

	/**
	 * Returns whether `this` is greater than `rhs`, when `rhs` is converted to
	 * an `Int`.
	 *
	 * @param {Value} rhs - The value to compare to
	 * @return {boolean} - Whether `this` is numerically greater than `rhs`.
	 */
	gth(rhs) {
		return this._data > rhs.toNumber();
	}
}

TYPES.push(Int);
