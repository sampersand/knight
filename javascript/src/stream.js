export class Stream {
	#source;

	constructor(source) {
		this.#source = source;
	}

	stripWhitespace() {
		this.#source = this.#source.replace(/^(?:[\]\[\s(){}:]+|\#[^\n]*(\n|$))*/, '');
	}

	match(regex, idx=0) {
		const match = regex.exec(this.#source);

		if (match === null) {
			return null;
		}

		this.#source = this.#source.substr(match[0].length);
		return match[idx];
	}

	toString() { return this.#source; }
}
