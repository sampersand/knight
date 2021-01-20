<?php
namespace Knight;

use \Knight\Value;

class Boolean extends Value
{
	public static function parse(Stream $stream): ?Value
	{
		$match = $stream->match('[TF][A-Z]*');

		return $match ? new self($match[0] === 'T') : null;
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

	protected function dataEql(Boolean $rhs): bool
	{
		return $this->data === $rhs->data;
	}
}
