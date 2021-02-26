export const TYPES = [];

export class Value {
	static parse(stream) {
		stream.stripWhitespace();

		for (var i = 0; i < TYPES.length; i++) {
			const match = TYPES[i].parse(stream);

			if (match) {
				return match;
			}
		}

		return null;
	}

	lth(rhs) {
		return this.cmp(rhs) < 0;
	}

	gth(rhs) {
		return this.cmp(rhs) > 0;
	}

	run() {
		return this;
	}

	toString() {
		return this.run().toString();
	}

	toInt() {
		return this.run().toInt();
	}

	toBool() {
		return this.run().toBool();
	}
}
