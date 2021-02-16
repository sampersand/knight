import { Value } from './Value.js';

export class Func extends Value {
	static FUNCTIONS = {};

	#args;
	#function;

	constructor(name, ...args) {
		super();

		if (!FUNCTIONS.hasOwnProperty(name)) {
			throw `Unknown function given: ${name}`;
		}

		this.#function = FUNCTIONS[name];
		this.#args = args;
	}

	static register(name, func)  {
		if (name.length !== 1) {
			throw `name must be exactly one character long, not ${name.length}`;
		}

		FUNCTIONS[name] = func;
	}

	run() {
		return this.#function(...this.#args);
	}

}

//register("")
console.log(random());
