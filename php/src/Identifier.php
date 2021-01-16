<?php
namespace Knight;

use \Knight\Value;

class Identifier extends Value
{
	private static $VARIABLES = [];

	public static function parse(string &$stream): ?Value
	{
		if (!preg_match('/\A[a-z_][a-z_0-9]*/', $stream, $match)) {
			return null;
		}

		$stream = preg_replace('/^[a-z0-9_]+/', '', $stream);

		return new self($match[0]);
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

	protected function _dataEql(Value $rhs): bool {
		die("this cant be calle d, as identifiers are always evaluated.");
	}

}

Value::$TYPES[] = Identifier::class;
