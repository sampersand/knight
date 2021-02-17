import { Value } from './value.js';
import { Bool } from './bool.js';
import { Ident } from './ident.js';
import { Int } from './int.js';
import { Nil } from './nil.js';
import { Str } from './str.js';
import { Stream } from './stream.js';

const FUNCTIONS = {};

export class Func extends Value {
	#args;
	#func;

	static parse(stream) {
		const match = stream.match(/^([A-Z]+|.)/);

		if (!match) {
			return;
		}

		const funcname = match[0]; // only the first character.
		const func = FUNCTIONS[funcname];

		if (func === undefined) {
			throw `Unknown function '${funcname}'`;
		}

		let args = [];
		for (let i = 0; i < func.length; i++) {
			const arg = Value.parse(stream);

			if (!arg) {
				throw new Error(`Missing argument ${i} for func '${funcname}'`);
			}

			args.push(arg);
		}

		return new Func(func, ...args);
	}

	constructor(func, ...args) {
		super();

		this.#func = func;
		this.#args = args;
	}

	run() {
		return this.#func(...this.#args);
	}
}

Value.TYPES.push(Func);

export function register(name, func)  {
	if (name.length !== 1) {
		throw `name must be exactly one character long, not ${name.length}`;
	}

	FUNCTIONS[name] = func;
}

register('P', () => { throw "todo" });
register('R', () => new Ident(Math.floor(Math.random() * 0xffff_ffff)));

import { exec } from 'child_process';
register('E', str => knight.run(str.toString()));
register('B', block => block);
register('C', block => block.run().run());
register('`', block => { throw "todo"; } ); //exec(block.toString(), block.run().run());
register('Q', code => process.exit(code.toInt()));
register('!', arg => new Bool(!arg.toBool()));
register('L', str => new Int(arg.toString().length));
register('O', input => {
	const result = input.run();
	const str = result.toString();

	if (str.substr(-1) === '\\') {
		process.stdout.write(str.substr(0, str.length - 1));
	} else {
		process.stdout.write(`${str}\n`);
	}

	return result;
});

register('+', (l, r) => l.run().add(r.run()));
register('-', (l, r) => l.run().sub(r.run()));
register('*', (l, r) => l.run().mul(r.run()));
register('/', (l, r) => l.run().div(r.run()));
register('%', (l, r) => l.run().mod(r.run()));
register('^', (l, r) => l.run().pow(r.run()));
register('<', (l, r) => new Bool(l.run().lth(r.run())));
register('>', (l, r) => new Bool(l.run().gth(r.run())));
register('?', (l, r) => new Bool(l.run().eql(r.run())));
register('&', (l, r) => l.run() && r.run());
register('|', (l, r) => l.run() || r.run());
register(';', (l, r) => (l.run(), r.run()));
register('=', (i, v) => {
	if (!(i instanceof Ident)) {
		i = new Ident(i.toString());
	}

	v = v.run();
	i.assign(v);
	return v;
});
register('W', (c, b) => {
	let ret;

	while (b.toBool()) {
		ret = c.run();
	}

	return ret || new Nil(); 
});

register('I', (cond, iftrue, iffalse) => cond.toBool() ? iftrue.run() : iffalse.run());
register('G', (str, start, len) => new Str(str.toString().substr(start.toInt(), len.toInt())));
register('S', (str, start, len, repl) => {
	str = str.toString();
	start = start.toInt(); 
	len = len.toInt(); 
	repl = repl.toString();

	return new Str(str.replace(str.substr(start, len), repl));
});
