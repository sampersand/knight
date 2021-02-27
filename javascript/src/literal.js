import { Value } from './value.js';

/**
 * The string type within Knight, used to represent textual data.
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

	toString() {
		return String(this._data);
	}

	toInt() {
		return Number(this._data);
	}

	toBool() {
		console.log(this._data, Boolean(this._data));
		return Boolean(this._data);
	}

	eql(rhs) {
		return rhs instanceof this.constructor && this._data === rhs._data;
	}
}
