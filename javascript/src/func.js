import { Value, TYPES } from './value.js';
import { Bool } from './bool.js';
import { Ident } from './ident.js';
import { Int } from './int.js';
import { Null } from './null.js';
import { Str } from './str.js';
import { Stream } from './stream.js';
import { ParseError } from './error.js';

const FUNCTIONS = {};

export class Func extends Value {
	#args;
	#func;
	#name;

	static parse(stream) {
		const func = FUNCTIONS[stream.peek()];

		if (func === null) {
			return null;
		}

		if (/[A-Z]/.test(stream.peek())) {
			stream.match(/^[A-Z]+/);
		} else {
			stream.match(/^./);
		}

		let args = [];
		for (let i = 0; i < func.length; i++) {
			const arg = Value.parse(stream);

			if (!arg) {
				throw new ParseError(`Missing argument ${i+1} for function '${match}'`);
			}

			args.push(arg);
		}

		return new Func(func, match, ...args);
	}

	constructor(func, name,...args) {
		super();

		this.#name =name;
		this.#func = func;
		this.#args = args;
	}

	run() {
		return this.#func(...this.#args);
	}

	dump() {
		let ret = 'Function(' + this.#name;

		for (let val of this.#args) {
			ret += ', ' + val.dump();
		}

		return ret + ')';
	}

	eql(rhs) { 
		return Object.is(this, rhs);
	}
}

// Add the `Func` class to the list of known types, so it can be parsed.	
TYPES.push(Func);

export function register(name, func)  {
	if (name.length !== 1) {
		throw new Error(`name must be exactly one character long, not ${name.length}`);
	}

	FUNCTIONS[name] = func;
}

import { readSync } from 'fs';
import { execSync } from 'child_process';
import { run } from './knight.js';

register('P', () => {
	let line = '';
	let buf = Buffer.alloc(1);

	do {
		readSync(0, buf, 0, 1);
		if (buf[0] == 0x00) {
			break;
		}

		line += buf;
	} while (buf[0] != 0x0a);

	return new Str(line);
});

register('R', () => new Int(Math.floor(Math.random() * 0xffff_ffff)));

register('E', string => run(string.toString()));
register('B', block => block);
register('C', block => block.run().run());
register('`', block => new Str(execSync(block.toString()).toString()));
register('Q', status => process.exit(status.toNumber()));
register('!', arg => new Bool(!arg.toBoolean()));
register('L', str => new Int(str.toString().length));
register('D', value => {
	const result = value.run();

	process.stdout.write(result.dump());

	return result;
});

register('O', input => {
	const result = input.run();
	const str = result.toString();

	process.stdout.write(str.substr(-1) === '\\' ? str.substr(0, str.length - 1) :`${str}\n`);
 
	return result;
});

register('+', (lhs, rhs) => lhs.run().add(rhs.run()));
register('-', (lhs, rhs) => lhs.run().sub(rhs.run()));
register('*', (lhs, rhs) => lhs.run().mul(rhs.run()));
register('/', (lhs, rhs) => lhs.run().div(rhs.run()));
register('%', (lhs, rhs) => lhs.run().mod(rhs.run()));
register('^', (lhs, rhs) => lhs.run().pow(rhs.run()));
register('<', (lhs, rhs) => new Bool(lhs.run().lth(rhs.run())));
register('>', (lhs, rhs) => new Bool(lhs.run().gth(rhs.run())));
register('?', (lhs, rhs) => new Bool(lhs.run().eql(rhs.run())));
register('&', (lhs, rhs) => {
	lhs = lhs.run();
	return lhs.toBoolean() ? rhs.run() : lhs;
});
register('|', (lhs, rhs) => {
	lhs = lhs.run();
	return lhs.toBoolean() ? lhs : rhs.run();
});
register(';', (lhs, rhs) => (lhs.run(), rhs.run()));
register('=', (ident, value) => {
	if (!(ident instanceof Ident)) {
		ident = new Ident(ident.toString());
	}

	return ident.assign(value);
});

register('W', (condition, body) => {
	let ret;

	while (condition.toBoolean()) {
		ret = body.run();
	}

	return ret || new Null(); 
});

register('I', (cond, iftrue, iffalse) => {
	return cond.toBoolean() ? iftrue.run() : iffalse.run();
});
register('G', (str, start, len) => {
	str = str.toString();
	start = start.toNumber();
	len = len.toNumber();

	return new Str(str.substr(start, len));
});


register('S', (str, start, len, repl) => {
	str = str.toString();
	start = start.toNumber(); 
	len = len.toNumber(); 
	repl = repl.toString();

	return new Str(str.replace(str.substr(start, len), repl));
}); 
