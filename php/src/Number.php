<?php
namespace Knight;

require_once 'Value.php';
use \Knight\Value;

class Number extends Value
{
	public static function parse(string &$stream): ?Value
	{
		if (!preg_match('/\A\d+/', $stream, $match)) {
			return null;
		}

		$stream = substr($stream, strlen($match[0]));

		return new self((int) $match[0]);
	}

	private $data;

	function __construct(int $val)
	{
		$this->data = $val;
	}

	function __toString(): string
	{
		return (string) $this->data;
	}

	public function toInt(): int
	{
		return $this->data;
	}

	public function toBool(): bool
	{
		return (bool) $this->data;
	}

	protected function _dataEql(Value $rhs): bool {
		return $this->data === $rhs->data;
	}
}

Value::$TYPES[] = Number::class;
