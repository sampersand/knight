<?php
namespace Knight;

abstract class Value
{
	private static $TYPES = [
		\Knight\Identifier::class,
		\Knight\Number::class,
		\Knight\Str::class,
		\Knight\Boolean::class,
		\Knight\Nil::class,
		\Knight\Func::class
	];

	public static function parse(Stream $stream): ?Value
	{
		$stream->strip();

		foreach (Value::$TYPES as $class) {
			if (!is_null($value = $class::parse($stream))) {
				return $value;
			}
		}

		return null;
	}

	abstract public function toInt(): int;
	abstract public function toBool(): bool;

	public function run(): Value
	{
		return clone $this;
	}

	public function lth(Value $rhs): bool {
		return $this->cmp($rhs) < 0;
	}

	public function gth(Value $rhs): bool {
		return $this->cmp($rhs) > 0;
	}

	public function eql(Value $rhs): bool
	{
		return get_class($this) === get_class($rhs) && $this->dataEql($rhs);
	}

}

