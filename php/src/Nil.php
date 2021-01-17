<?php
namespace Knight;

use \Knight\Value;

class Nil extends Value
{
	public static function parse(string &$stream): ?Value
	{
		if (!preg_match('/\AN/', $stream)) {
			return null;
		}

		$stream = preg_replace('/^[A-Z]*/', '', $stream);

		return new self();
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

	protected function dataEql(Value $rhs): bool
	{
		return true;
	}
}
