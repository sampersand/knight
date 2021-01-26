<?php
namespace Knight;

/**
 * A Stream that's used when parsing Values.
 **/
class Stream
{
	/**
	 * The stream of text that's to be parsed.
	 *
	 * @var string
	 **/
	private $stream;

	/**
	 * Creates a new Stream for the given $source
	 *
	 * @var string $source The string to use as the source for this Stream.
	 **/
	public function __construct(string $source)
	{
		$this->source = $source;
	}

	/**
	 * Removes all leading whitespace and comments.
	 *
	 * Note that, for Knight, all forms of parens (ie `[`, `]`, `(`, `)`, `{`, and `}`), as well as the colon (`:`) are
	 * considered whitespace.
	 *
	 * @return void
	 **/
	public function strip(): void
	{
		$this->source = preg_replace('/\A(?:[\]\[\s(){}:]+|\#[^\n]*(\n|$))*/m', '', $this->source);
	}

	/**
	 * Attempts to match the $regex at the start of the source.
	 *
	 * If the regex matches, the entire matching string will be returned by default. The `$idx` parameter can be used to
	 * change this behaviour around.
	 *
	 * @param string $regex The regex to match at the beginning of the string; It will have the `/m` flag applied.
	 * @param int $idx The index of the group to return; defaults to `0`, ie the entire match.
	 * @return string|null Returns the matching string (or the `$idx` capture group) if `$regex` matched. Else, null.
	 **/
	public function match(string $regex, int $idx=0): ?string
	{
		if (!preg_match("/\A(?:$regex)/m", $this->source, $match)) {
			return null;
		}

		$this->source = substr($this->source, strlen($match[0]));

		return $match[$idx];
	}
}
