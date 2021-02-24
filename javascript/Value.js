
export class Value {
	lth(rhs) { return this.run().cmp(rhs.run()) < 0; }
	gth(rhs) { return this.run().cmp(rhs.run()) > 0; }
	run() { return this; }

	toString() {
		this.run().toString();
	}

	toInteger() {
		this.run().toInteger();
	}

	toBoolean() {
		this.run().toBoolean();
	}
}
