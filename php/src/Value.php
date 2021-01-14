<?php
namespace Knight;

abstract class Value {
	public static function parse(string &$stream): ?Value {
		return Text::parse($stream) ?? 3;
	}

	abstract public function toInt(): int;
	abstract public function toBool(): bool;

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
		return new Number($this->toInt() - $rhs->toInt());
	}

	public function div(Value $rhs): Value
	{
		$lhs = $this->toInt();
		$rhs = $rhs->toInt();

		if ($rhs === 0) {
			throw new Exception("Cannot divide by zero");
		}

		return new Number(intdiv($this->toInt(), $rhs->toInt()));
	}

	public function mod(Value $rhs): Value
	{
		$lhs = $this->toInt();
		$rhs = $rhs->toInt();

		if ($rhs === 0) {
			throw new Exception("Cannot modulo by zero");
		}

		return new Number($this->toInt() % $rhs->toInt());
	}

	public function pow(Value $rhs): Value
	{
		return new Number($this->toInt() ** $rhs->toInt());
	}

	public function cmp(Value $rhs): int
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
		return $this === get_class($rhs) && $this->data === $rhs->data;
	}
}
