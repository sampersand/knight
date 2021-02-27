import { Value, TYPES } from './value';
import { Literal } from './literal';
import { RuntimeError } from './error';
import { Stream } from './stream';

export class Int extends Literal<number> {
	static parse(stream: Stream): (null | Int) {
		const match = stream.match(/^\d+/);

		return match && new Int(Number(match));
	}

	dump(): string {
		return `Number(${this})`;
	}

	add(rhs: Value): Value {
		return new Int(this.data + rhs.toInt());
	}

	sub(rhs: Value): Value {
		return new Int(this.data - rhs.toInt());
	}

	mul(rhs: Value): Value {
		return new Int(this.data * rhs.toInt());
	}

	div(rhs: Value): Value {
		const rhsInt = rhs.toInt();

		if (rhsInt !== 0) {
			return new Int(Math.trunc(this.data / rhsInt));
		} else {
			throw new RuntimeError('Cannot divide by zero');
		}
	}

	mod(rhs: Value): Value {
		const rhsInt = rhs.toInt();

		if (rhsInt !== 0) {
			return new Int(this.data % rhsInt);
		} else {
			throw new RuntimeError('Cannot modulo by zero');
		}
	}

	pow(rhs: Value): Value {
		return new Int(Math.trunc(Math.pow(this.data, rhs.toInt())));
	}

	lth(rhs: Value): boolean {
		return this.data < rhs.toInt();
	}

	gth(rhs: Value): boolean {
		return this.data > rhs.toInt();
	}
}

TYPES.push(Int);
