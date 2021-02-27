import { Value } from './value.js';

/**
 * An abstract class that's used 
 *
 * @see Value - For more information on why we don't simply use `String`.
 * @extends {Literal<string>}
 */
export class Literal extends Value {
	_data;

	constructor(data) {
		super();

		this._data = data;
	}

	run() {
		return this;
	}

	toString() {
		return String(this._data);
	}

	toNumber() {
		return Number(this._data);
	}

	toBoolean() {
		return Boolean(this._data);
	}

	eql(rhs) {
		return rhs instanceof this.constructor && this._data === rhs._data;
	}
}
