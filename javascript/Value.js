export class Value {
	static TYPES = [];

	static parse(stream) {
		for (var i = 0; i < Value.TYPES.length; i++) {
			const match = Value.TYPES[i].parse(stream);

			if (match !== null) {
				return match;
			}
		}

		return null;
	}

	lth(rhs) {
		return this.run().cmp(rhs.run()) < 0;
	}

	gth(rhs) {
		return this.run().cmp(rhs.run()) > 0;
	}

	run() {
		return this;
	}

	toString() {
		this.run().toString();
	}

	toInt() {
		this.run().toInt();
	}

	toBool() {
		this.run().toBool();
	}
}

//import { Bool } from './Bool.js';
