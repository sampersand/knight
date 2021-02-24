<?php
namespace Knight;

/**
 * The boolean type within Knight.
 **/
class Boolean extends Value
{
	/**
	 * Attempt to parse a Boolean from the given stream.
	 *
	 * @param Stream $stream The stream to read from.
	 * @return null|Value Returns the parsed Boolean if it's able to be parsed, otherwise `null`.
	 **/
	public static function parse(Stream $stream): ?Value
	{
		$match = $stream->match('[TF][A-Z]*');

		return $match ? new self($match[0] === 'T') : null;
	}

	/**
	 * This Boolean's value.
	 *
	 * @var bool
	 **/
	private $data;

	/**
	 * Create a new Boolean with the given value.
	 *
	 * @param bool $val The value of this Boolean.
	 **/
	public function __construct(bool $val)
	{
		$this->data = $val;
	}

	/**
	 * Converts this Boolean to a string.
	 *
	 * @return string Either "true" or "false", depending on whether this is true or false.
	 **/
	public function __toString(): string
	{
		return $this->data ? 'true' : 'false';
	}

	/**
	 * Converts this Boolean to an int.
	 *
	 * @return int Either 0 or 1, depending on whether this is true or false.
	 **/
	public function toInt(): int
	{
		return (int) $this->data;
	}

	/**
	 * Converts this Boolean to a bool.
	 *
	 * @return bool Simply returns the data associated with this class.
	 **/
	public function toBool(): bool
	{
		return $this->data;
	}

	/**
	 * Gets a string representation of this class, for debugging purposes.
	 *
	 * @return string
	 **/
	public function dump(): string
	{
		return "Boolean($this)";
	}

	/**
	 * Converts the $rhs to an boolean, then compares $this to it.
	 *
	 * This will only return `true` if `$this` is false and `$rhs` is true, or `$this` is true and `$rhs` is false.
	 *
	 * @param Value $rhs The boolean to compare to.
	 * @return int Returns a number less than, equal to, or greater than 0, depending on if `$rhs`, after conversion to
	 * an int, is less than, equal to, or greater than `$this`.
	 **/
	protected function cmp(Value $rhs): int
	{
		return $this->data <=> $rhs->toBool();
	}	

	/**
	 * Checks to see if `$value` is a `Boolean` and equal to `$this`.
	 *
	 * @return bool
	 **/
	public function eql(Value $value): bool
	{
		return is_a($value, get_class()) && $this->data === $value->data;
	}
}
