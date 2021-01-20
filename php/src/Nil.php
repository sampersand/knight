<?php
namespace Knight;

use \Knight\Value;

class Nil extends Value
{
	public static function parse(Stream $stream): ?Value
	{
		return is_null($stream->match('N[A-Z]*')) ? null : new self();
	}

	function __toString(): string
	{
		return 'null';
	}

	public function toInt(): int
	{
		return 0;
	}

	public function toBool(): bool
	{
		return false;
	}

	protected function dataEql(Nil $rhs): bool
	{
		return true;
	}
}
