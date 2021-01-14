<?php

namespace Knight;

require_once __DIR__ . DIRECTORY_SEPARATOR . 'Value.php';
use \Knight\Value;

function run($input): Value {
	$torun = (string) $input;
	return Value::parse($torun)->run();
}

require_once __DIR__ . DIRECTORY_SEPARATOR . 'Nil.php';
require_once __DIR__ . DIRECTORY_SEPARATOR . 'Number.php';
require_once __DIR__ . DIRECTORY_SEPARATOR . 'Text.php';
require_once __DIR__ . DIRECTORY_SEPARATOR . 'Boolean.php';
require_once __DIR__ . DIRECTORY_SEPARATOR . 'Function.php';
