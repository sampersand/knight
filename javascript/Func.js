import { Value } from './value.js';
import { Bool } from './bool.js';
import { Ident } from './ident.js';
import { Int } from './int.js';
import { Nil } from './nil.js';
import { Str } from './str.js';
import { Stream } from './stream.js';

export class Func extends Value {
	static FUNCTIONS = {};

	#args;
	#function;

	static parse(stream) {
		const match = stream.match(/^([A-Z]+|.)/);

		if (match === null) {
			return null;
		}

		const funcname = match[0]; // only the first character.
		const func = Func.FUNCTIONS[funcname];

		if (func === undefined) {
			throw `Unknown function '${funcname}'`;
		}

		let args = [];

		for (let i = 0; i < func.length; i++) {

		}

		// return match && new Bool(match);
	}

	constructor(name, ...args) {
		super();

		if (!Func.FUNCTIONS.hasOwnProperty(name)) {
			throw `Unknown function given: ${name}`;
		}

		this.#function = Func.FUNCTIONS[name];
		this.#args = args;
	}

	static register(name, func)  {
		if (name.length !== 1) {
			throw `name must be exactly one character long, not ${name.length}`;
		}

		Func.FUNCTIONS[name] = func;
	}

	run() {
		return this.#function(...this.#args);
	}
}

Value.TYPES.push(Func);

Func.register('P', () => { throw "todo" });
Func.register('R', () => new Ident(Math.floor(Math.random() * 0xffff_ffff)));

import { exec } from 'child_process';
Func.register('E', str => knight.run(str));
Func.register('B', block => block);
Func.register('C', block => block.run().run());
Func.register('`', block => { throw "todo"; } ); //exec(block.toString(), block.run().run());
Func.register('Q', code => process.exit(code.toInt()));
Func.register('!', arg => new Bool(!arg.toBool()));
Func.register('L', str => new Int(arg.toString().length));
Func.register('O', str => {
	let res = str.run();
	str = str.toString();

	if (str.substr(-1) === '\\') {
		process.stdout.write(str.substr(0, str.length - 1));
	} else {
		process.stdout.write(`${str}\n`);
	}

	return res;
});

Func.register('+', (l, r) => l.run().add(r.run()));
Func.register('-', (l, r) => l.run().sub(r.run()));
Func.register('*', (l, r) => l.run().mul(r.run()));
Func.register('/', (l, r) => l.run().div(r.run()));
Func.register('%', (l, r) => l.run().mod(r.run()));
Func.register('^', (l, r) => l.run().pow(r.run()));
Func.register('<', (l, r) => new Bool(l.run().lth(r.run())));
Func.register('>', (l, r) => new Bool(l.run().gth(r.run())));
Func.register('?', (l, r) => new Bool(l.run().eql(r.run())));
Func.register('&', (l, r) => l.run() && r.run());
Func.register('|', (l, r) => l.run() || r.run());
Func.register(';', (l, r) => (l.run(), r.run()));
Func.register('=', (i, v) => {
	if (!(i instanceof Ident)) {
		i = new Ident(i.toString());
	}

	v = v.run();
	i.assign(v);
	return v;
});
Func.register('W', (c, b) => {
	let ret;
	while (b.toBool()) {
		ret = c.run();
	}

	return ret || new Nil(); 
});

Func.register('I', (cond, iftrue, iffalse) => cond.toBool() ? iftrue.run() : iffalse.run());
Func.register('G', (str, start, len) => new Str(str.toString().substr(start.toInt(), len.toInt())));
Func.register('S', (str, start, len, repl) => {
	str = str.toString();
	start = start.toInt(); 
	len = len.toInt(); 
	repl = repl.toString();

	return new Str(str.replace(str.substr(start, len), repl));
});
