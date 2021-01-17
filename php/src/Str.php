<?php
namespace Knight;

use \Knight\Value;

class Str extends Value
{
	public static function parse(string &$stream): ?Value
	{
		switch (1) {
			case preg_match("/\A'([^']*)'/", $stream, $match):
			case preg_match('/\A"([^"]*)"/', $stream, $match):
				$stream = substr($stream, strlen($match[0]));

				return new self($match[1]);
			case preg_match('/\A[\'"]/', $stream):
				throw new \Exception('Unterminated quote encountered!');
			default:
				return null;
		} 
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

	protected function _dataEql(Value $rhs): bool
	{
		return $this->data === $rhs->data;
	}
}
