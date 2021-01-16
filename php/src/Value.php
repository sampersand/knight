<?php
namespace Knight;

abstract class Value {
	static $TYPES = [];

	public static function parse(string &$stream): ?Value {
		$stream = preg_replace('/\A(?:[\]\[\s(){}:]+|\#[^\n]*\n)*/', '', $stream);

		foreach (Value::$TYPES as $class) {
			if (!is_null($value = $class::parse($stream))) {
				// print_r($value);
				return $value;
			}
		}

		return Func::parse($stream);
	}

	abstract public function toInt(): int;
	abstract public function toBool(): bool;
	abstract protected function _dataEql(Value $rhs): bool;

 	public function __call(string $name, array $arguments) {
 		die("unknown function '$name'.");
 	}

	public function run(): Value {
		return clone $this;
	}

	public function add(Value $rhs): Value
	{
		return new Number($this->toInt() + $rhs->toInt());
	}

	public function sub(Value $rhs): Value
	{
		return new Number($this->toInt() - $rhs->toInt());
	}

	public function mul(Value $rhs): Value
	{
		return new Number($this->toInt() * $rhs->toInt());
	}

	public function div(Value $rhs): Value
	{
		$lhs = $this->toInt();
		$rhs = $rhs->toInt();

		if ($rhs === 0) {
			throw new \Exception("Cannot divide by zero");
		}

		return new Number(intdiv($this, $rhs));
	}

	public function mod(Value $rhs): Value
	{
		$lhs = $this->toInt();
		$rhs = $rhs->toInt();

		if ($rhs === 0) {
			throw new \Exception("Cannot modulo by zero");
		}

		return new Number($this % $rhs);
	}

	public function pow(Value $rhs): Value
	{
		return new Number($this->toInt() ** $rhs->toInt());
	}

	protected function cmp(Value $rhs): int
	{
		return $this->toInt() <=> $rhs->toInt();
	}

	public function lth(Value $rhs): bool
	{
		return $this->cmp($rhs) < 0;
	}

	public function gth(Value $rhs): bool
	{
		return $this->cmp($rhs) > 0;
	}

	public function eql(Value $rhs): bool
	{
		return get_class($this) === get_class($rhs) && $this->_dataEql($rhs);
	}

}
