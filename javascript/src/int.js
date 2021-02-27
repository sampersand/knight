import { Value, TYPES } from './value.js';
import { Literal } from './literal.js';

export class Int extends Literal {
	static parse(stream) {
		const match = stream.match(/^\d+/);

		return match && new Int(Number(match));
	}

	dump() {
		return `Number(${this})`;
	}

	add(rhs) {
		return new Int(this._data + rhs.toInt());
	}

	sub(rhs) {
		return new Int(this._data - rhs.toInt());
	}

	mul(rhs) {
		return new Int(this._data * rhs.toInt());
	}

	div(rhs) {
		const rhsInt = rhs.toInt();

		if (rhsInt !== 0) {
			return new Int(Math.trunc(this._data / rhsInt));
		} else {
			throw new RuntimeError('Cannot divide by zero');
		}
	}

	mod(rhs) {
		const rhsInt = rhs.toInt();

		if (rhsInt !== 0) {
			return new Int(this._data % rhsInt);
		} else {
			throw new RuntimeError('Cannot modulo by zero');
		}
	}

	pow(rhs) {
		return new Int(Math.trunc(this._data ** rhs.toInt()));
	}

	lth(rhs) {
		return this._data < rhs.toInt();
	}

	gth(rhs) {
		return this._data > rhs.toInt();
	}
}

TYPES.push(Int);
