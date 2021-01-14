<?php
namespace Knight;

require_once 'Value.php';
use \Knight\Value;
require_once 'Number.php';
require_once 'Nil.php';
require_once 'Text.php';
require_once 'Boolean.php';


class Func
{
	private static $KNOWN = [];

	public static function fetch(string $name): ?Func
	{
		return Func::$KNOWN[$name];
	}

	public static function register(string $name, int $arity, callable $func)
	{
		if (strlen($name) != 1) {
			throw new Exception('Name must be exactly one letter long');
		}

		Func::$KNOWN[$name] = new self($arity, $func);
	}

	private $arity;
	private $func;

	private function __construct($arity, $func)
	{
		$this->arity = $arity;
		$this->func = $func;
	}

	public function getArity(): int
	{
		return $this->arity;
	}

	public function run(...$params): Value {
		return ($this->func)(...$params);
	}
}

Func::register('P', 0, function() {
	return new Text(chop(fgets(STDIN)));
});

Func::register('R', 0, function() {
	return new Number(random_int(0, 0xffffffff));
});

require_once 'Knight.php';

$x="'3'";
echo Func::fetch('E')->run(Value::parse($x));

Func::register('E', 1, function(Value $text) {
	return \Knight::run((string) $text->run());
});

Func::register('I', 3, function(Value $cond, Value $iftrue, Value $iffalse) {
	return ($cond->run()->toBool() ? $iftrue : $iffalse)->run();
});


// 		define 'EVAL' do |what|
// 			Value.new(what.to_s).run
// 		end

// 		define 'BLOCK' do |block|
// 			block
// 		end

// 		define 'CALL' do |block|
// 			block.run.run
// 		end

// 		define '`', 'SYSTEM' do |cmd|
// 			`#{cmd}`
// 		end

// 		define 'QUIT' do |code|
// 			exit code.to_i
// 		end

// 		define '!', 'NOT' do |arg|
// 			!arg
// 		end

// 		define 'LENGTH' do |arg|
// 			arg.to_s.length
// 		end

// 		define 'OUTPUT' do |msg|
// 			text = (msg = msg.run).to_s

// 			if text.end_with? '\\'
// 				print text[..-2]
// 			else
// 				puts text
// 			end

// 			msg
// 		end

// 		define '+', 'ADD' do |lhs, rhs|
// 			lhs + rhs
// 		end

// 		define '-', 'SUB' do |lhs, rhs|
// 			lhs - rhs
// 		end

// 		define '*', 'MUL' do |lhs, rhs|
// 			lhs * rhs
// 		end

// 		define '/', 'DIV' do |lhs, rhs|
// 			lhs / rhs
// 		end

// 		define '%', 'MOD' do |lhs, rhs|
// 			lhs % rhs
// 		end

// 		define '^', 'POW' do |lhs, rhs|
// 			lhs ** rhs
// 		end

// 		define '<', 'LTH' do |lhs, rhs|
// 			lhs < rhs
// 		end

// 		define '>', 'GTH' do |lhs, rhs|
// 			lhs > rhs
// 		end

// 		define '?', 'EQL' do |lhs, rhs|
// 			lhs == rhs
// 		end

// 		define '&', 'AND' do |lhs, rhs|
// 			(lhs = lhs.run).to_b ? rhs.run : lhs
// 		end

// 		define '|', 'OR' do |lhs, rhs|
// 			(lhs = lhs.run).to_b ? lhs : rhs.run
// 		end

// 		define ';', 'THEN' do |lhs, rhs|
// 			lhs.run
// 			rhs.run
// 		end

// 		define 'WHILE' do |cond, body|
// 			ret = body.run while cond.to_b
// 			ret
// 		end

// 		define '=', 'ASSIGN' do |var, arg|
// 			Kn::ENVIRONMENT[var.is_a?(Identifier) ? var.data : var.to_s] = arg.run
// 		end

// 		define 'IF' do |cond, ift, iff|
// 			(cond.to_b ? ift : iff).run
// 		end

// 		define 'GET' do |string, start, count|
// 			string.to_s[start.to_i, count.to_i]
// 		end

// 		define 'SET' do |string, start, count, repl|
// 			ret = string.to_s.dup
// 			ret[start.to_i, count.to_i] = repl.to_s
// 			ret
// 		end
// 	end
// end
