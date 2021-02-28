import { readSync } from 'fs';
import { execSync } from 'child_process';

import { Value, TYPES } from './value.js';
import { Bool } from './bool.js';
import { Ident } from './ident.js';
import { Int } from './int.js';
import { Null } from './null.js';
import { Str } from './str.js';
import { Stream } from './stream.js';
import { ParseError } from './error.js';
import { run } from './knight.js';

/**
 * @typedef {import('./stream.js').Stream} Stream
 */

/**
 * The list of all known functions.
 *
 * @private
 * @type {Object.<string, function>}
 * @see {@link register} for how to add new functions.
 */
const FUNCTIONS = {};

/**
 * A class that represents a function and its associated arguments.
 */
export class Func extends Value {
	/**
	 * The array of arguments associated with this function, if any.
	 *
	 * @private
	 * @type {Value[]}
	 */
	#args;

	/**
	 * The function to call when running this instance.
	 *
	 * @private
	 * @type {function}
	 */
	#func;

	/**
	 * The name associated with this instance.
	 *
	 * This variable is only used within the `dump` function.
	 *
	 * @private
	 * @type {string}
	 */
	#name;

	/**
	 * Attempts to parse a `Func` from `stream`.
	 *
	 * @param {Stream} stream - The stream from which to parse.
	 * @return {Func|null} - The parsed `Func`, or `null` if the stream did not
	 *                       start with a valid function name.
	 * @throws {ParseError} - If not all the arguments could be parsed.
	 */
	static parse(stream) {
		const front = stream.peek();
		const func = FUNCTIONS[front];

		if (func === undefined) {
			return null;
		}

		// Remove either the beginning upper-case word or symbol.
		stream.match(/^(?:[A-Z]+|.)/); // ignore the result; we have `func`.

		// Parse the arguments of the function.
		let args = [];
		for (let i = 0; i < func.length; i++) { // `func.length` is its arity.
			const arg = Value.parse(stream);

			if (!arg) {
				throw new ParseError(`Missing argument ${i+1} for func '${front}'`);
			}

			args.push(arg);
		}

		return new Func(func, front, args);
	}

	/**
	 * Creates a new `Func` with the given arguments.
	 *
	 * @param {function} func - The function execute when running `this`.
	 * @param {string} name - The name of this function, used when `dump`ing.
	 * @param {Value[]} args - The list of arguments to use when running.
	 */
	constructor(func, name, args) {
		super();

		this.#func = func;
		this.#name = name;
		this.#args = args;
	}

	/**
	 * Returns the result of executing the function associated with `this`.
	 *
	 * Note that the arguments are passed unevaluated to the function.
	 *
	 * @return {Value} - The result of executing `this`.
	 */
	run() {
		return this.#func(...this.#args);
	}

	/**
	 * Provides a debugging representation of this class.
	 *
	 * @return {string}
	 */
	dump() {
		let ret = 'Function(' + this.#name;

		for (let val of this.#args) {
			ret += ', ' + val.dump();
		}

		return ret + ')';
	}

	/**
 	 * Checks to see if `rhs` is _identical_ to `this`.
 	 *
 	 * @param {Value} rhs - The value to compare against.
 	 * @return {boolean} - Whether `this` and `rhs` are the same object.
	 */
	eql(rhs) {
		return Object.is(this, rhs);
	}
}

// Add the `Func` class to the list of known types, so it can be parsed.
TYPES.push(Func);

/**
 * Associates `name` with the function `func`, such that `Func.parse` recognizes
 * it.
 *
 * @param {string} name - The name of the function; must be one letter long.
 * @param {function} func - The function to associate with `name`.
 * @throw {Error} - Thrown when `name` is not exactly one letter long.
 */
export function register(name, func)  {
	if (name.length !== 1) {
		throw new Error('Name must be exactly one character long.');
	}

	FUNCTIONS[name] = func;
}

/***
 * Yes, I realize these JSDocs aren't actually commenting on the functions
 * themselves, but I think it's valuable to provide documentation nonetheless.
 ***/

/**
 * Reads a line from stdin, retaining trailing newlines.
 *
 * @return {Str} - The line read from stdin.
 */
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

/**
 * Gets a random number from `0` through `0xffff_ffff`.
 *
 * @return {Int} - A random number.
 */
register('R', () => new Int(Math.floor(Math.random() * 0x1_0000_0000)));

/**
 * Evaluates `string` as a piece of Knight code.
 *
 * @param {Value} string - The string to evaluate.
 * @return {Value} - The result of executing the string.
 */
register('E', string => run(string.toString()));

/**
 * Simply returns its argument unevaluated.
 *
 * @param {Value} block - The value to return.
 * @return {Value} - Literally just `block`, unchanged.
 */
register('B', block => block);

/**
 * Calls its argument, as per the Knight specs.
 *
 * @param {Value} block - The value to run.
 * @return {Value} - The result of calling `block`.
 */
register('C', block => block.run().run());

/**
 * Runs its argument as a shell command, returning the stdout of the command.
 *
 * @param {Value} command - The command to run.
 * @return {Str} - The stdout of `command`.
 */
register('`', command => new Str(execSync(command.toString()).toString()));

/**
 * Exits the program with the given status code.
 *
 * @param {Value} status - The status code to exit with.
 */
register('Q', status => process.exit(status.toNumber()));

/**
 * Converts `arg` to a boolean, then returns its negation.
 *
 * @param {Value} arg - The value to invert.
 * @return {Bool} - The logical negation of `arg`.
 */
register('!', arg => new Bool(!arg.toBoolean()));

/**
 * Returns the amount of chars in `str`, after it's converted to a string.
 *
 * @param {Value} str - The string whose chars will be counted.
 * @return {Int} - The amount characters iwthin `str`.
 */
register('L', str => new Int(str.toString().length));

/**
 * Writes the debug representation of `value` to stdout, without a trailing
 * newline, and then returns `value`.
 *
 * @param {Value} value - The value to dump to stdout.
 * @return {Int} - Simply `value` after running it.
 */
register('D', value => {
	const result = value.run();

	process.stdout.write(result.dump());

	return result;
});

/**
 * Writes `input` to stdout.
 *
 * If `input`, when converted to a string, ends with a backslash, the backslash
 * will be removed and no trailing newline will be added. Otherwise, a newline
 * will be printed after `input`.
 *
 * @param {Value} input - The value to write to stdout.
 * @return {Null}
 */
register('O', input => {
	const str = input.toString();

	if (str.substr(-1) === '\\') {
		process.stdout.write(str.substr(0, str.length - 1));
	} else{
		process.stdout.write(`${str}\n`);
	}

	return new Null();
});

/**
 * Adds `lhs` and `rhs` together.
 *
 * @param {Value} lhs - The augend.
 * @param {Value} rhs - The addend.
 * @return {Value} - The summation `rhs + lhs`.
 */
register('+', (lhs, rhs) => lhs.run().add(rhs.run()));

/**
 * Subtracts `lhs` from `rhs`.
 *
 * @param {Value} lhs - The minuend.
 * @param {Value} rhs - The subtrahend.
 * @return {Value} - The difference `rhs - lhs`.
 */
register('-', (lhs, rhs) => lhs.run().sub(rhs.run()));

/**
 * Multiplies `lhs` and `rhs` together.
 *
 * @param {Value} lhs - The multiplicand.
 * @param {Value} rhs - The multiplier.
 * @return {Value} - The product `lhs * rhs`.
 */
register('*', (lhs, rhs) => lhs.run().mul(rhs.run()));

/**
 * Divides `rhs` by `lhs`.
 *
 * @param {Value} lhs - The dividend.
 * @param {Value} rhs - The divisor.
 * @return {Value} - The quotient `lhs / rhs`.
 */
register('/', (lhs, rhs) => lhs.run().div(rhs.run()));

/**
 * Modulos `rhs` by `lhs`.
 *
 * @param {Value} lhs - The base.
 * @param {Value} rhs - The divisor.
 * @return {Value} - The value `lhs (mod rhs)`.
 */
register('%', (lhs, rhs) => lhs.run().mod(rhs.run()));

/**
 * Raises `rhs` to the `lhs`th power.
 *
 * @param {Value} lhs - The base.
 * @param {Value} rhs - The exponent.
 * @return {Value} - The value `lhs ^ rhs`.
 */
register('^', (lhs, rhs) => lhs.run().pow(rhs.run()));

/**
 * Checks to see if `rhs` is less than `lhs`.
 *
 * @param {Value} lhs - The value to compare with.
 * @param {Value} rhs - The value to compare against.
 * @return {boolean} - Whether `lhs` is less than `rhs` or not.
 */
register('<', (lhs, rhs) => new Bool(lhs.run().lth(rhs.run())));

/**
 * Checks to see if `rhs` is greater than `lhs`.
 *
 * @param {Value} lhs - The value to compare with.
 * @param {Value} rhs - The value to compare against.
 * @return {boolean} - Whether `lhs` is greater than `rhs` or not.
 */
register('>', (lhs, rhs) => new Bool(lhs.run().gth(rhs.run())));

/**
 * Checks to see if `rhs` is equal to `rhs`.
 *
 * @param {Value} lhs - The value to compare with.
 * @param {Value} rhs - The value to compare against.
 * @return {boolean} - Whether `lhs` is equivalent to `rhs`.
 */
register('?', (lhs, rhs) => new Bool(lhs.run().eql(rhs.run())));

/**
 * Returns `rhs` or `lhs`, depending on whether `lhs` is truthy or not.
 *
 * @param {Value} lhs - The value to check and return if falsey.
 * @param {Value} rhs - The value to return if `lhs` is truthy.
 * @return {Value} - When `lhs` is truthy, `rhs`; otherwise, `lhs`.
 */
register('&', (lhs, rhs) => {
	lhs = lhs.run();
	return lhs.toBoolean() ? rhs.run() : lhs;
});

/**
 * Returns `lhs` or `rhs`, depending on whether `lhs` is truthy or not.
 *
 * @param {Value} lhs - The value to check and return if truethy.
 * @param {Value} rhs - The value to return if `lhs` is false.
 * @return {Value} - When `lhs` is truthy, `lhs`; otherwise, `rhs`.
 */
register('|', (lhs, rhs) => {
	lhs = lhs.run();
	return lhs.toBoolean() ? lhs : rhs.run();
});

/**
 * Runs `lhs`, then runs and returns `rhs`.
 *
 * @param {Value} lhs - The first value to run.
 * @param {Value} rhs - The second value to run.
 * @return {Value} - The result of running `rhs`.
 */
register(';', (lhs, rhs) => {
	lhs.run();
	return rhs.run();
});

/**
 * Assigns `value` to `ident`.
 *
 * @param {Value} ident - The name of the variable. If not an `Ident`, it will
 *                        be converted to a string automatically.
 * @param {Value} value - The value to assign to `ident`.
 * @return {Value} - The result of running `value`.
 */
register('=', (ident, value) => {
	if (!(ident instanceof Ident)) {
		ident = new Ident(ident.toString());
	}

	ident.assign(value = value.run());
	return value;
});

/**
 * Executes the `body`` whilst the `condition` is true.
 *
 * @param {Value} condition - The condition under which to continue the loop.
 * @param {Value} body - The value that's to be executed during the loop.
 * @return {Null}
 */
register('W', (condition, body) => {
	while (condition.toBoolean()) {
		body.run();
	}

	return new Null();
});

/**
 * Executes and returns `iftrue` or `iffalse` based on `cond`.
 *
 * @param {Value} cond - The condition that's used to decide what to return.
 * @param {Value} iftrue - The value to execute and return if `cond` is truthy.
 * @param {Value} iffalse - The value to execute and return if `cond` is falsey.
 * @return {Value} - The result of running `iftrue` or `iffalse`.
 */
register('I', (cond, iftrue, iffalse) => {
	return cond.toBoolean() ? iftrue.run() : iffalse.run();
});

/**
 * Gets the specified substring of `str`.
 * 
 * See the Knight specs for details about edge cases.
 *
 * @param {Value} str - The string to get a substring of.
 * @param {Value} start - The beginning index of the substring.
 * @param {Value} len - The length of the substring.
 * @return {Str} - The specified substring.
 */
register('G', (str, start, len) => {
	str = str.toString();
	start = start.toNumber();
	len = len.toNumber();

	return new Str(str.substr(start, len) || "");
});

/**
 * Returns a new string with the specified range of `str` replaced with `repl`.
 * 
 * See the Knight specs for details about edge cases.
 *
 * @param {Value} str - The base string to replace.
 * @param {Value} start - The beginning index of the replacement.
 * @param {Value} len - The length of the replacement.
 * @param {Value} repl - The value to substitute for the specified range.
 */
register('S', (str, start, len, repl) => {
	str = str.toString();
	start = start.toNumber();
	len = len.toNumber();
	repl = repl.toString();

	return new Str(str.replace(str.substr(start, len), repl) || "");
});
