<?php
namespace Knight;

/**
 * The string type within Knight.
 *
 * Note this class is named `Str` because `String` is not a valid class name.
 **/
class Str extends Value
{
	/**
	 * Attempt to parse a Str from the given stream.
	 *
	 * @param Stream $stream The stream to read from.
	 * @return null|Value Returns the parsed Str if it's able to be parsed, otherwise `null`.
	 **/
	public static function parse(Stream $stream): ?Value
	{
		$match = $stream->match("([\"'])((?:(?!\\1).|\n)*)\\1", 2);

		if (is_null($match)) {
			if ($stream->match("[\"']")) {
				throw new \Exception('Unterminated quote encountered!');
			} else {
				return null;
			}
		}

		return new self($match);
	}

	/**
	 * This Str's value.
	 *
	 * @var string
	 **/
	private $data;

	/**
	 * Create a new Str with the given value.
	 *
	 * @param string $val The value of this Str.
	 **/
	public function __construct(string $val)
	{
		$this->data = $val;
	}

	/**
	 * Converts this Str to a string.
	 *
	 * @return bool Simply returns the data associated with this class.
	 **/
	public function __toString(): string
	{
		return $this->data;
	}

	/**
	 * Converts this Str to an int.
	 *
	 * @return int Simply converts to an int using PHP's conversion rules, which are the same as Knight's.
	 **/
	public function toInt(): int
	{
		// Avoid php's scientific notation by manually grepping.
		if (!preg_match("/\A\s*[-+]?\d+/m", $this->data, $match)) {
			return 0;
		} else {
			return (int) $match[0];
		}
	}

	/**
	 * Converts this Str to an bool.
	 *
	 * @return bool An empty string is considered false; everything else (including `"0"` is considered true).
	 **/
	public function toBool(): bool
	{
		return $this->data !== '';
	}

	/**
	 * Gets a string representation of this class, for debugging purposes.
	 *
	 * @return string
	 **/
	public function dump(): string
	{
		return "String($this->data)";
	}

	/**
	 * Converts $rhs to a string and then adds it to the end of $this.
	 *
	 * @param Value $rhs The value concatenate to this.
	 * @return string `$this` concatenated with `$rhs` converted to a string.
	 **/
	public function add(Value $rhs): Value
	{
		return new Str($this . $rhs);
	}

	/**
	 * Converts $rhs to an int, then repeats $this that many times.
	 *
	 * For example, `new Str("ab")->mul(new Str("3"))` will return `ababab`. If `$rhs` is zero, then an empty string will
	 * be returned.
	 *
	 * @param Value $rhs The value by which `$this` will be duplicated.
	 * @return string `$this` duplicated `$rhs` times.
	 **/
	public function mul(Value $rhs): Value
	{
		return new Str(str_repeat($this, $rhs->toInt()));
	}

	/**
	 * Converts the $rhs to an string, then lexicographically compares $this to it.
	 *
	 * @param Value $exponent The string by which `$this` will be raised.
	 * @return int Returns a number less than, equal to, or greater than 0, depending on if `$rhs`, after conversion to
	 * an int, is less than, equal to, or greater than `$this`.
	 **/
	protected function cmp(Value $rhs): int
	{
		return strcmp($this, $rhs);
	}

	/**
	 * Checks to see if `$value` is a `Str` and equal to `$this`.
	 *
	 * @return bool
	 **/
	public function eql(Value $value): bool
	{
		return is_a($value, get_class()) && $this->data === $value->data;
	}
}
