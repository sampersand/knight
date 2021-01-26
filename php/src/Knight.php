<?php
namespace Knight;

require_once __DIR__  . '/Value.php';
use \Knight\Value;

/**
 * Parses a Value from the input, and then executes it.
 *
 * Because every Knight program is _technically_ a single expression, this will parse one Value, then execute it.
 *
 * @param string $input The string to parse and execute.
 * @return Value The result of executing `$input`.
 * @throws Exception Throws an exception if no Values are able to be parsed from the `$input`.
 * @throws Exception Any exceptions that are thrown during execution are also raised.
 **/
function run(string $input): Value
{
	return Value::parse(new \Knight\Stream($input))->run();
}

// I don't have composer installed... So I used this hack instead.
require_once __DIR__  . '/Stream.php';
require_once __DIR__  . '/Nil.php';
require_once __DIR__  . '/Number.php';
require_once __DIR__  . '/Str.php';
require_once __DIR__  . '/Boolean.php';
require_once __DIR__  . '/Identifier.php';
require_once __DIR__  . '/Func.php';
