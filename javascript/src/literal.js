import { Value } from './value.js';

/**
 * An abstract class that's used to indicate a type is a wrapper around a
 * JavaScript built-in.
 *
 * @template {T}
 * @abstract
 * @see Value - For more information on why we don't simply use built-ins.
 */
export class Literal extends Value {
	/**
	 * The data associated with this Literal.
	 *
	 * @protected
	 * @type {T}
	 */
	_data;

	/**
	 * Creates a new `Literal` with the given data.
	 *
	 * @param {T} data - The data to associate with this `Literal`.
	 */
	constructor(data) {
		super();

		this._data = data;
	}

	/**
	 * Running a literal simply returns itself.
	 *
	 * @return {Literal} - `this`.
	 */
	run() {
		return this;
	}

	/**
	 * Converts this class's `_data` to a JavaScript `string`.
	 *
	 * @return {string}
	 */
	toString() {
		return String(this._data);
	}

	/**
	 * Converts this class's `_data` to a JavaScript `number`.
	 *
	 * @return {number}
	 */
	toNumber() {
		return Number(this._data);
	}

	/**
	 * Converts this class's `_data` to a JavaScript `boolean`.
	 *
	 * @return {boolean}
	 */
	toBoolean() {
		return Boolean(this._data);
	}

	/**
	 * A `Literal` is only equal to another instance of the same type_, which
	 * also has the same `_data`.
	 *
	 * @param {Value} rhs - The value to compare against `this`.
	 * @return {boolean} - Whether `rhs` is a `this.constructor`, and its `_data`
	 *                     is equivalent to `this`'s.
	 */
	eql(rhs) {
		return rhs instanceof this.constructor && this._data === rhs._data;
	}
}
