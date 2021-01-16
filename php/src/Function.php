<?php
namespace Knight;

use \Knight\Value;

class Func extends Value
{
	private static $KNOWN = [];

	public static function register(string $name, int $arity, callable $func)
	{
		if (strlen($name) != 1) {
			throw new \Exception('Name must be exactly one letter long');
		}

		Func::$KNOWN[$name] = [$func, $arity];
	}

	public static function parse(string &$stream): ?Value
	{
		if (!preg_match('/\A([A-Z]|\S)/', $stream, $match)) {
			return null;
		}

		[$func, $arity] = Func::$KNOWN[$match[0]];

		if (!isset($func)) {
			throw new \Exception("unknown function '$match[0]'");
		}

		$stream = preg_replace('/^([A-Z]+|\S)/', '', $stream);
		$args = [];

		for ($i=0; $i < $arity; $i++) { 
			$next = Value::parse($stream);

			if (!isset($next)) {
				throw new \Exception("missing argument $i for function '$match[0]'");
			}

			$args[] = $next;
		}

		return new self($func, $args);
	}

	private $func;
	private $args;

	private function __construct(callable $func, array $args)
	{
		$this->func = $func;
		$this->args = $args;
	}

	public function run(): Value
	{
		return ($this->func)(...$this->args);
	}

	public function __toString(): string
	{
		return (string) $this->run();
	}

	public function toInt(): int
	{
		return $this->run()->toInt();
	}

	public function toBool(): bool
	{
		return $this->run()->toBool();
	}

	protected function _dataEql(Value $rhs): bool {
		die("this can't be called, as functions are always evaluated.");
	}
}

Func::register('P', 0, function(): Value {
	return new Text(chop(fgets(STDIN)));
});

Func::register('R', 0, function(): Value {
	return new Number(random_int(0, 0xffffffff));
});

Func::register('E', 1, function(Value $text): Value {
	return \Knight\run((string) $text->run());
});

Func::register('B', 1, function(Value $block): Value {
	return $block;
});

Func::register('C', 1, function(Value $block): Value {
	return $block->run()->run();
});

Func::register('`', 1, function(Value $command): Value {
	return new Text(shell_exec($command->run()));
});

Func::register('Q', 1, function(Value $block): Value {
	exit($block->toInt());
});

Func::register('!', 1, function(Value $block): Value {
	return new Boolean(!$block->toBool());
});

Func::register('L', 1, function(Value $block): Value {
	return new Number(strlen($block->run()));
});

Func::register('O', 1, function(Value $message): Value {
	$message = $message->run();
	$string = (string) $message;

	if (substr($string, -1) === '\\') {
		echo substr($string, 0, -1);
	} else {
		echo $string . PHP_EOL;
	}

	return $message;
});

Func::register('+', 2, function(Value $lhs, Value $rhs): Value {
	return $lhs->run()->add($rhs->run());
});

Func::register('-', 2, function(Value $lhs, Value $rhs): Value {
	return $lhs->run()->sub($rhs->run());
});

Func::register('*', 2, function(Value $lhs, Value $rhs): Value {
	return $lhs->run()->mul($rhs->run());
});

Func::register('/', 2, function(Value $lhs, Value $rhs): Value {
	return $lhs->run()->div($rhs->run());
});

Func::register('%', 2, function(Value $lhs, Value $rhs): Value {
	return $lhs->run()->mod($rhs->run());
});

Func::register('^', 2, function(Value $lhs, Value $rhs): Value {
	return $lhs->run()->pow($rhs->run());
});

Func::register('<', 2, function(Value $lhs, Value $rhs): Value {
	return new Boolean($lhs->run()->lth($rhs->run()));
});

Func::register('>', 2, function(Value $lhs, Value $rhs): Value {
	return new Boolean($lhs->run()->gth($rhs->run()));
});

Func::register('?', 2, function(Value $lhs, Value $rhs): Value {
	return new Boolean($lhs->run()->eql($rhs->run()));
});

Func::register('&', 2, function(Value $lhs, Value $rhs): Value {
	$lhs = $lhs->run();

	return $lhs->toBool() ? $rhs->run() : $lhs;
});

Func::register('|', 2, function(Value $lhs, Value $rhs): Value {
	$lhs = $lhs->run();

	return $lhs->toBool() ? $lhs : $rhs->run();
});

Func::register(';', 2, function(Value $lhs, Value $rhs): Value {
	$lhs->run();
	return $rhs->run();
});

Func::register('W', 2, function(Value $cond, Value $body): Value {
	$ret = null;

	while ($cond->run()->toBool()) {
		$ret = $body->run();
	}

	return is_null($ret) ? new Nil() : $ret;
});

Func::register('=', 2, function(Value $var, Value $val): Value {
	if (!is_a($var, '\Knight\Identifier')) {
		$var = new Identifier((string) $var->run());
	}

	$val = $val->run();

	$var->assign($val);

	return $val;
});

Func::register('I', 3, function(Value $cond, Value $iftrue, Value $iffalse): Value {
	return ($cond->run()->toBool() ? $iftrue : $iffalse)->run();
});

Func::register('G', 3, function(Value $string, Value $start, Value $length): Value {
	return new Text(substr($string->run(), $start->run()->toInt(), $length->run()->toInt()));
});

Func::register('S', 4, function(Value $string, Value $start, Value $length, Value $replacement): Value {
	$string = $string->run();
	$start = $start->run()->toInt();
	$length = $length->run()->toInt();
	$replacement = $replacement->run();

	// maybe
	return new Text(substr_replace($string, $replacement, $start, $length));
});
