import { Stream } from './stream';
export const TYPES = [];

export abstract class Value {
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

	abstract run(): Value;
	abstract dump(): string;

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
