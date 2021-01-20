<?php
namespace Knight;

use \Knight\Value;

class Identifier extends Value
{
	private static $VARIABLES = [];

	public static function parse(Stream $stream): ?Value
	{
		$match = $stream->match('[a-z_][a-z_0-9]*');

		return is_null($match) ? null : new self($match);
	}

	private $data;

	function __construct(string $data)
	{
		$this->data = $data;
	}

	public function run(): Value
	{
		return self::$VARIABLES[$this->data] ?? die("unknown variable '$this->data'!");
	}

	public function assign(Value $value): void
	{
		self::$VARIABLES[$this->data] = $value;
	}

	function __toString(): string
	{
		return (string) $this->run();
	}

	public function toInt(): int
	{
		return $this->run()->toInt();
	}

	public function toBool(): bool
	{
		return $this->run()->toBool();
	}
}
