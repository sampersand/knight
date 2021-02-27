import { Value, TYPES } from './value';
import { Bool } from './bool';
import { Ident } from './ident';
import { Int } from './int';
import { Null } from './null';
import { Str } from './str';
import { Stream } from './stream';
import { ParseError } from './error';

const FUNCTIONS = {};

export class Func extends Value {
	args;
	func;
	name;

	static parse(stream) {
		const match = stream.match(/^([A-Z\p{P}\p{S}])(?:(?<=[A-Z])[A-Z]*)?/u, 1);

		if (match === null) {
			return;
		}

		const func = FUNCTIONS[match];

		if (!func) {
			throw new ParseError(`Unknown function '${match}'`);
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

		this.name =name;
		this.func = func;
		this.args = args;
	}

	run() {
		return this.func(...this.args);
	}

	dump() {
		let ret = 'Function(' + this.name;

		for (let val of this.args) {
			ret += ', ' + val.dump();
		}

		return ret + ')';
	}

	eql(rhs) { 
		return Object.is(this, rhs);
	}
}

TYPES.push(Func);

export function register(name, func)  {
	if (name.length !== 1) {
		throw new Error(`name must be exactly one character long, not ${name.length}`);
	}

	FUNCTIONS[name] = func;
}

import { readSync } from 'fs';
import { execSync } from 'child_process';
import { run } from './knight';

register('P', () => {
	let line = '';
	let buf = Buffer.alloc(1);

	do {
		// readSync(0, buf, 0, 1);
		if (buf[0] == 0x00) {
			break;
		}

		line += buf;
	} while (buf[0] != 0x0a);

	return new Str(line);
});

register('R', () => new Int(Math.floor(Math.random() * 0xffffffff)));

register('E', string => run(string.toString()));
register('B', block => block);
register('C', block => block.run().run());
register('`', block => new Str(execSync(block.toString()).toString()));
register('Q', status => process.exit(status.toInt()));
register('!', arg => new Bool(!arg.toBool()));
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
	return lhs.toBool() ? rhs.run() : lhs;
});
register('|', (lhs, rhs) => {
	lhs = lhs.run();
	return lhs.toBool() ? lhs : rhs.run();
});
register(';', (lhs, rhs) => (lhs.run(), rhs.run()));
register('=', (ident, value) => {
	if (!(ident instanceof Ident)) {
		ident = new Ident(ident.toString());
	}

	const ran = value.run();
	ident.assign(ran);

	return ran;
});
register('W', (condition, body) => {
	let ret;

	while (condition.toBool()) {
		ret = body.run();
	}

	return ret || new Null(); 
});

register('I', (cond, iftrue, iffalse) => {
	return cond.toBool() ? iftrue.run() : iffalse.run();
});
register('G', (str, start, len) => {
	str = str.toString();
	start = start.toInt();
	len = len.toInt();

	return new Str(str.substr(start, len));
});


register('S', (str, start, len, repl) => {
	str = str.toString();
	start = start.toInt(); 
	len = len.toInt(); 
	repl = repl.toString();

	return new Str(str.replace(str.substr(start, len), repl));
}); 
