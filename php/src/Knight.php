<?php

namespace Knight;

require_once __DIR__ . DIRECTORY_SEPARATOR . 'Value.php';
use \Knight\Value;

function run(string $input): Value {
	return Value::parse($input)->run();
}

return;
// include_once '../src/Boolean.php';
// include_once '../src/Number.php';
// include_once '../src/Text.php';
// include_once '../src/Nil.php';
// include_once '../src/Function.php';

require_once __DIR__ . DIRECTORY_SEPARATOR . 'Nil.php';
require_once __DIR__ . DIRECTORY_SEPARATOR . 'Number.php';
require_once __DIR__ . DIRECTORY_SEPARATOR . 'Text.php';
require_once __DIR__ . DIRECTORY_SEPARATOR . 'Boolean.php';
require_once __DIR__ . DIRECTORY_SEPARATOR . 'Function.php';
