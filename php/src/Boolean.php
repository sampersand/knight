<?php
namespace Knight;

require_once 'Value.php';
use \Knight\Value;

class Boolean extends Value
{
	public static function parse(string &$stream): ?Value
	{
		if (!preg_match('/\A[TF]/', $stream, $match)) {
			return null;
		}

		$stream = preg_replace('/^[A-Z]*/', '', $stream);

		return new self($match[0] === 'T');
	}

	private $data;

	function __construct(bool $val)
	{
		$this->data = $val;
	}

	function __toString(): string
	{
		return $this->data ? "true" : "false";
	}

	public function toInt(): int
	{
		return (int) $this->data;
	}

	public function toBool(): bool
	{
		return $this->data;
	}

	protected function _dataEql(Value $rhs): bool {
		return $this->data === $rhs->data;
	}
}

Value::$TYPES[] = Boolean::class;
