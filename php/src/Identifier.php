<?php
namespace Knight;

/**
 * Identifiers within Knight.
 *
 * As per the specs for Knight, all variables are global scope.
 **/
class Identifier extends Value
{
	/**
	 * The list of all known identifiers and their associated values.
	 *
	 * @var array[string => Value] 
	 **/
	private static $VARIABLES = [];

	/**
	 * Attempt to parse an Identifier from the given stream.
	 *
	 * @param Stream $stream The stream to read from.
	 * @return null|Value Returns the parsed Identifier if it's able to be parsed, otherwise `null`.
	 **/
	public static function parse(Stream $stream): ?Value
	{
		$match = $stream->match('[a-z_][a-z_0-9]*');

		return is_null($match) ? null : new self($match);
	}

	/**
	 * This Identifier's value.
	 *
	 * @var string
	 **/
	private $data;

	/**
	 * Create a new Identifier with the given value.
	 *
	 * @param string $val The text of this identifier.
	 **/
	public function __construct(string $data)
	{
		$this->data = $data;
	}

	/**
	 * Looks up this variable in the list of known variables, returning its most recently assigned value.
	 *
	 * @return Value The most recent value associated with this variable.
	 * @throws Exception Thrown if the variable has not been set yet.
	 **/
	public function run(): Value
	{
		$value = self::$VARIABLES[$this->data];

		if (isset($value)) {
			return $value;
		} else {
			throw new Exception("unknown variable '$this->data'!");
		}
	}

	/**
	 * Assigns a value to this identifier.
	 *
	 * The previous value is discarded.
	 *
	 * @param Value $value The value to assign to this identifier.
	 * @return void
	 **/
	public function assign(Value $value): void
	{
		self::$VARIABLES[$this->data] = $value;
	}

	/**
	 * Fetches this identifier's value, then converts it to a string.
	 *
	 * @return string The string representation of the value associated with this identifier.
	 * @throws Exception Thrown if the variable has not been set yet.
	 **/
	public function __toString(): string
	{
		return (string) $this->run();
	}

	/**
	 * Fetches this identifier's value, then converts it to an int.
	 *
	 * @return int The result of calling `toInt` on the value associated with this identifier.
	 * @throws Exception Thrown if the variable has not been set yet.
	 **/
	public function toInt(): int
	{
		return $this->run()->toInt();
	}

	/**
	 * Fetches this identifier's value, then converts it to a bool.
	 *
	 * @return bool The result of calling `toBool` on the value associated with this identifier.
	 * @throws Exception Thrown if the variable has not been set yet.
	 **/
	public function toBool(): bool
	{
		return $this->run()->toBool();
	}

	/**
	 * Gets a string representation of this class, for debugging purposes.
	 *
	 * @return string
	 **/
	public function dump(): string
	{
		return "Identifier($this->data)";
	}

	/**
	 * Checks to see if `$value` is an `Identifier` and equal to `$this`.
	 *
	 * @return bool
	 **/
	public function eql(Value $value): bool
	{
		return $this === $value;
	}
}
