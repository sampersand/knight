<?php
namespace Knight;

/**
 * The null type within Knight.
 *
 * Unlike every other `Value`, `Nil` doesn't take an argument in its constructor---this is because there is only one
 * kind of `Nil`.
 *
 * Note this class is named `Nil` because `Null` is not a valid class name.
 **/
class Nil extends Value
{
	/**
	 * Attempt to parse a Nil from the given stream.
	 *
	 * @param Stream $stream The stream to read from.
	 * @return null|Value Returns the parsed Nil if it's able to be parsed, otherwise `null`.
	 **/
	public static function parse(Stream $stream): ?Value
	{
		return is_null($stream->match('N[A-Z]*')) ? null : new self();
	}

	/**
	 * Converts this `Nil` to a string.
	 *
	 * @return string Simply `'null'`.
	 **/
	public function __toString(): string
	{
		return 'null';
	}

	/**
	 * Converts this `Nil` to an int.
	 *
	 * @return int Simply `0`.
	 **/
	public function toInt(): int
	{
		return 0;
	}

	/**
	 * Converts this `Nil` to an boolean.
	 *
	 * @return int Simply `false`.
	 **/
	public function toBool(): bool
	{
		return false;
	}
}
