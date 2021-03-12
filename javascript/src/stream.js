/**
 * The source code of a Knight program.
 *
 * When parsing Knight source code, there needs to be a way to communicate what
 * part of the source code was parsed. However, because JavaScript does not have
 * out parameters, nor does it allow for the modification of strings, we must
 * use a class to maintain the remaining source code to be parsed. Thus, the
 * `Stream` class.
 *
 * @see Value.parse
 */
export class Stream {
	/** @type {string} */
	#source;

	/**
	 * Creates a new stream with the given source.
	 * @param {string} source - The source of the stream.
	 */
	constructor(source) {
		this.#source = source;
	}

	/**
	 * Strips the leading whitespace and comments from `this`.
	 */
	stripWhitespace() {
		// simply ignore the return value--it can fail for all we care.
		this.match(/^([\]\[\s(){}:]+|#[^\n]*(\n|$))*/);
	}

	/**
	 * Peeks at the first character of the stream, without consuming it
	 *
	 * @return {string|null} - The first character of `this`, or `null` empty.
	 */
	peek() {
		return this.#source[0] || null;
	}

	/**
	 * Attempts to match the given `regex` at the start of the stream, returning
	 * the `group`th group if successful.
	 *
	 * @param {RegExp} regex - The regular expression to match, which should have
	 *                         an `^` (so as to only match the stream start).
	 * @param {number} [group] - The group number to return; the default (0)
	 *                           returns the entire match.
	 * @return {string|null} - Returns the matched group, or `null` if no match.
	 */
	match(regex, group=0) {
		const match = regex.exec(this.#source);

		if (match === null) {
			return null;
		}

		// remove the match from the source code.
		this.#source = this.#source.substr(match[0].length);

		return match[group];
	}

	/**
	 * Returns the remainder of the stream to be parsed.
	 *
	 * @return {string} - The remainder of the stream.
	 */
	toString() {
		return this.#source;
	}
}
