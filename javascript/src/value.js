/**
 * @typedef {import('./stream.js').Stream} Stream
 */

/**
 * The list of known types; used when parsing `Value`s.
 *
 * When parsing, this array will be traversed in order; the first matching
 * value will be returned.
 *
 * @type {Type[]}
 * @see Value.parse
 */
export const TYPES = [];

/**
 * The parent class for all entities within Knight source code.
 *
 * While JavaScript itself has builtin types for booleans, numbers, strings,
 * and null, their semantics do not _exactly_ match Knight's: e.g., converting
 * `" a"` to a number in Knight should return `0`, where in JavaScript it is
 * `NaN`. Another exmaple is how, in Knight, `+ <number> <anything>` should
 * convert the `<anything>` to a number, whereas in JavaScript it will not if
 * `<anything>` is a string.
 *
 * As such, I've opted for have these wrapper classes. An alternative to this
 * could have been to have th `Func`tions define the semantics for each type
 * through a `switch` statement. However, that doesn't really feel like
 * idiomatic JavaScript, so I've opted for this.
 *
 * @abstract
 */
export class Value {
	/**
	 * Attempts to parse out a new value from the given `stream`.
	 *
	 * @param {Stream} stream - The stream to parse.
	 * @return {Value|null} - Returns the parsed value, or `null` if nothing
	 *                        could be parsed.
	 */
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

	/**
	 * Returns the result of evaluating `this`.
	 *
	 * @abstract
	 */
	run() {
		throw new Error;
	}

	/**
	 * Returns debugging information for `this`.
	 *
	 * @abstract
	 * @return {string}
	 */
	debug() {
		throw new Error;
	}

	/**
	 * Converts this Value to a JavaScript string.
	 *
	 * The default implementation simply calls `toString()` on `run()`'s result.
	 *
	 * @return {string}
	 */
	toString() {
		return this.run().toString();
	}

	/**
	 * Converts this Value to a JavaScript number.
	 *
	 * The default implementation simply calls `toNumber()` on `run()`'s result.
	 *
	 * @return {number}
	 */
	toNumber() {
		return this.run().toNumber();
	}

	/**
	 * Converts this Value to a JavaScript boolean.
	 *
	 * The default implementation simply calls `toBoolea()` on `run()`'s result.
	 *
	 * @return {boolean}
	 */
	toBoolean() {
		return this.run().toBoolean();
	}
}
