<?php

namespace Knight;

require_once __DIR__  . '/Value.php';
use \Knight\Value;

function run(string $input): Value
{
	return Value::parse($input)->run();
}

require_once __DIR__  . '/Nil.php';
require_once __DIR__  . '/Number.php';
require_once __DIR__  . '/Str.php';
require_once __DIR__  . '/Boolean.php';
require_once __DIR__  . '/Identifier.php';
require_once __DIR__  . '/Func.php';
