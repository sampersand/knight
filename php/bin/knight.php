#!/usr/bin/env php
<?php

include_once __DIR__ . '/../src/Knight.php';

switch(count($argv) === 3 ? $argv[1] : null) {
	case '-e':
		\Knight\run($argv[2]);
		break;
	case '-f':
		\Knight\run(file_get_contents($argv[2]));
		break;
	default:
		die("USAGE: $argv[0] (-e 'code' | -f file)\n");
}
