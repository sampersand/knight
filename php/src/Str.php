<?php
namespace Knight;

use \Knight\Value;

class Str extends Value
{
	public static function parse(Stream $stream): ?Value
	{
		$match = $stream->match("([\"'])((?:(?!\\1).|\n)*)\\1", 2);

		if (is_null($match)) {
			if ($stream->match("[\"']")) {
				throw new \Exception('Unterminated quote encountered!');
			} else {
				return null;
			}
		}

		return new self($match);
	}

	private $data;

	function __construct(string $val)
	{
		$this->data = $val;
	}

	function __toString(): string
	{
		return $this->data;
	}

	public function toInt(): int
	{
		return (int) $this->data;
	}

	public function toBool(): bool
	{
		return $this->data !== "";
	}

	public function add(Value $rhs): Value
	{
		return new Str($this . $rhs);
	}

	public function mul(Value $rhs): Value
	{
		return new Str(str_repeat($this, $rhs->toInt()));
	}

	protected function cmp(Value $rhs): int
	{
		return strcmp($this, $rhs);
	}

	protected function dataEql(Value $rhs): bool
	{
		return $this->data === $rhs->data;
	}
}
