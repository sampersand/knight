<?php
namespace Knight;

/**
 * The base class for all things that are representable within Knight.
 **/
abstract class Value
{
	/**
	 * All the different types that exist within Knight.
	 *
	 * @var class[]
	 **/
	private const TYPES = [
		Identifier::class,
		Number::class,
		Str::class,
		Boolean::class,
		Nil::class,
		Func::class
	];

	/**
	 * Attempts to parse a Value from the given Stream.
	 *
	 * If a value is found, the stream will be updated accordingly; if nothing can be parsed, `null` will be returned.
	 *
	 * @param Stream $stream The stream which will be parsed from.
	 * @return null|Value Returns the parsed Value, or null if nothing could be parsed.
	 **/
	public static function parse(Stream $stream): ?Value
	{
		$stream->strip();

		foreach (Value::TYPES as $class) {
			if (!is_null($value = $class::parse($stream))) {
				return $value;
			}
		}

		return null;
	}

	/**
	 * Converts this value to a int.
	 *
	 * @return int
	 **/
	abstract public function toInt(): int;

	/**
	 * Converts this value to a bool.
	 *
	 * @return bool
	 **/
	abstract public function toBool(): bool;

	/**
	 * Gets a string representation of this class, for debugging purposes.
	 *
	 * @return string
	 **/
	abstract public function dump(): string;

	/**
	 * Checks to see if `$this` is equal to `$value`.
	 *
	 * @return bool
	 **/
	abstract public function eql(Value $value): bool;

	/**
	 * Executes this Value.
	 *
	 * By default, the return value is simply `$this`.
	 *
	 * @return Value The result of running this value.
	 **/
	public function run(): Value
	{
		return $this;
	}

	/**
	 * Checks to see if this value is less than the other.
	 *
	 * This calls the `cmp` and then checks to make sure the value is less than zero.
	 *
	 * @param Value $rhs The value to test against.
	 * @return bool Returns `true` if `$this` is less than `$rhs`.
	 */
	public function lth(Value $rhs): bool
	{
		return $this->cmp($rhs) < 0;
	}

	/**
	 * Checks to see if this value is greater than the other.
	 *
	 * This calls the `cmp` and then checks to make sure the value is greater than zero.
	 *
	 * @param Value $rhs The value to test against.
	 * @return bool Returns `true` if `$this` is greater than `$rhs`.
	 */
	public function gth(Value $rhs): bool
	{
		return $this->cmp($rhs) > 0;
	}
}
