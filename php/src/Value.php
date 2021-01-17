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

	public static function parse(string &$stream): ?Value
	{
		$stream = preg_replace('/\A(?:[\]\[\s(){}:]+|\#[^\n]*\n)*/', '', $stream);

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

	public function eql(Value $rhs): bool
	{
		return get_class($this) === get_class($rhs) && $this->dataEql($rhs);
	}

}

