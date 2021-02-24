<?php
namespace Knight;

/**
 * The numeric type within Knight.
 *
 * As Knight only supports integral values, there's only a single number type.
 **/
class Number extends Value
{
	/**
	 * Attempt to parse a Number from the given stream.
	 *
	 * @param Stream $stream The stream to read from.
	 * @return null|Value Returns the parsed Number if it's able to be parsed, otherwise `null`.
	 **/
	public static function parse(Stream $stream): ?Value
	{
		$match = $stream->match('\d+');

		return is_null($match) ? null : new self((int) $match);
	}

	/**
	 * This Number's value.
	 *
	 * @var int
	 **/
	private $data;

	/**
	 * Create a new Number with the given value.
	 *
	 * @param int $val The value of this Number.
	 **/
	public function __construct(int $val)
	{
		$this->data = $val;
	}

	/**
	 * Converts this Number to a string.
	 *
	 * Note that the resulting string should not be run directly via eval (ie don't do `EVAL + "" number`). This is 
	 * because negative numbers will have a leading `-` and `-1` is interpreted (in Knight) as `- 1 <missing second
	 * operand>`.
	 *
	 * @return string Either "true" or "false", depending on whether this is true or false.
	 **/
	public function __toString(): string
	{
		return (string) $this->data;
	}

	/**
	 * Converts this Number to an int.
	 *
	 * @return int Simply returns the data associated with this class.
	 **/
	public function toInt(): int
	{
		return $this->data;
	}

	/**
	 * Converts this Number to a bool.
	 *
	 * @return bool Returns `false` if this number is zero, otherwise returns `true`.
	 **/
	public function toBool(): bool
	{
		return (bool) $this->data;
	}

	/**
	 * Gets a string representation of this class, for debugging purposes.
	 *
	 * @return string
	 **/
	public function dump(): string
	{
		return "Number($this)";
	}

	/**
	 * Converts the addend to an int, and then adds it to $this.
	 *
	 * @param Value $addend The number to add to `$this`.
	 * @return Value The result of the addition.
	 **/
	public function add(Value $addend): Value
	{
		return new self($this->data + $addend->toInt());
	}

	/**
	 * Converts the subtrahend to an int, and then subtract it from $this.
	 *
	 * @param Value $subtrahend The number to subtract from `$this`.
	 * @return Value The result of the subtraction.
	 **/
	public function sub(Value $subtrahend): Value
	{
		return new self($this->data - $subtrahend->toInt());
	}

	/**
	 * Converts the multiplicand to an int, and then multiplies $this by it.
	 *
	 * @param Value $multiplicand The number by which `$this` will be multiplied.
	 * @return Value The result of the multiplication.
	 **/
	public function mul(Value $multiplicand): Value
	{
		return new self($this->data * $multiplicand->toInt());
	}

	/**
	 * Converts the divisor to an int, and then divides $this by it.
	 *
	 * @param Value $divisor The number by which `$this` will be divided.
	 * @return Value The result of the division.
	 * @throws Exception If the divisor is zero.
	 **/
	public function div(Value $divisor): Value
	{
		$divisor = $divisor->toInt();

		if ($divisor === 0) {
			throw new \Exception("Cannot divide by zero");
		}

		return new self(intdiv($this->data, $divisor));
	}

	/**
	 * Converts the base to an int, and then modulos $this by it.
	 *
	 * @param Value $base The number by which `$this` will be modulo'd.
	 * @return Value The result of the modular division.
	 * @throws Exception If the base is zero.
	 **/
	public function mod(Value $base): Value
	{
		$base = $base->toInt();

		if ($base === 0) {
			throw new \Exception("Cannot modulo by zero");
		}

		return new self($this->data % $base);
	}

	/**
	 * Converts the exponent to an int, then raises $this by it.
	 *
	 * @param Value $exponent The number by which `$this` will be raised.
	 * @return Value The result of the exponentiation.
	 **/
	public function pow(Value $exponentiation): Value
	{
		return new self($this->data ** $exponentiation->toInt());
	}

	/**
	 * Converts the $rhs to an int, then compares $this to it.
	 *
	 * @param Value $rhs The number by which `$this` will be raised.
	 * @return int Returns a number less than, equal to, or greater than 0, depending on if `$rhs`, after conversion to
	 * an int, is less than, equal to, or greater than `$this`.
	 **/
	protected function cmp(Value $rhs): int
	{
		return $this->data <=> $rhs->toInt();
	}

	/**
	 * Checks to see if `$value` is a `Number` and equal to `$this`.
	 *
	 * @return bool
	 **/
	public function eql(Value $value): bool
	{
		return is_a($value, get_class()) && $this->data === $value->data;
	}
}
