require 'minitest'
require 'minitest/spec'
require 'stringio'
require_relative 'shared'

describe 'Function' do
	include Kn::Test::Shared

	describe 'parsing' do
		describe 'symbol functions' do
			it 'should only consume one character' do
				assert_equal false, eval('!1')
				assert_equal 2, eval('+1 1')
			end

			it 'will not consume additional symbolic chars' do
				assert_equal 5, eval('--9 3 1')
			end
		end

		describe 'keyword functions' do
			it 'should strip trailing uppercase chars' do
				assert_equal 3, eval('LABCDEFHIJKLMNOPQRSTUVWXYZ"foo"')
			end

			it 'should not strip trailing `_`s' do
				assert_equal 3, eval('; = _ 99 : + LENGTH_T') # 99 + true = 99 + 1 = 100
			end
		end
	end

	describe 'the functions' do
		describe 'RANDOM' do
			it 'should return a random value each time its called.' do
				assert_equal false, eval('? RANDOM RANDOM')
			end
		end

		describe 'PROMPT' do
			it 'should return a string without the \n or \r\n' do
				return pass
				old_stdin = $stdin
				$stdin = StringIO.new("line one\nline two\r\nline three")
				assert_equal "line one", eval("PROMPT")
				assert_equal "line two", eval("PROMPT")
				assert_equal "line three", eval("PROMPT")
			ensure
				$stdin = old_stdin
			end
		end

		describe 'EVAL' do
			it 'should evaluate text' do
				assert_equal 12, eval('EVAL "12"')
				assert_fails { eval('EVAL "missing identifier"') }
				assert_equal 12, eval('EVAL "12"') # convert to a string
			end

			it 'should convert values to a string' do
				assert_equal 23, eval('; = true 23 : EVAL TRUE')
				assert_equal 23, eval('; = false 23 : EVAL FALSE')
				assert_equal 23, eval('; = null 23 : EVAL NULL')
				assert_equal 19, eval('EVAL 19')
			end

			it 'should update the global scope' do
				assert_equal 591, eval('; EVAL "= foo 591" foo')
			end
		end

		describe 'BLOCK' do
			it 'should return the correct type' do
				assert_equal 12, eval('BLOCK 12')
				assert_equal "12", eval('BLOCK "12"')

				# NOTE: Depending on the implementation, this can either return a function or a literal.
				# This is OK, as the only valid operation on the return value of `BLOCK` is `CALL`,
				# which should evaluate the function/literal to a literal.
				assert [true, Kn::Test::Function].any? { |x| x === eval('TRUE') }
				assert [false, Kn::Test::Function].any? { |x| x === eval('FALSE') }
				assert [:null, Kn::Test::Function].any? { |x| x === eval('NULL') }

				assert_kind_of Kn::Test::Identifier, eval('BLOCK foo')
				assert_kind_of Kn::Test::Function, eval('BLOCK + 1 2')
			end

			it 'should not evaluate its argument' do
				assert_runs { 'BLOCK bar' }
			end
		end

		describe 'CALL' do
			it 'should evaluate something returned by `BLOCK`' do
				assert_equal 12, eval('CALL BLOCK 12')
				assert_equal "12", eval('CALL BLOCK "12"')
	
				# unlike `BLOCK`, the return value of these _must_ be a literal.
				assert_equal true, eval('CALL BLOCK TRUE')
				assert_equal false, eval('CALL BLOCK FALSE')
				assert_equal :null, eval('CALL BLOCK NULL')

				assert_equal "twelve", eval('; = foo BLOCK bar ; = bar "twelve" : CALL foo')
				assert_equal 15, eval('; = foo BLOCK * x 5 ; = x 3 : CALL foo')
			end
		end

		describe '`' do
			# TODO
		end

		describe 'Q' do
			def exit_code(expr)
				assert_silent do
					eval "CALL QUIT #{expr}"
				rescue
					nil
				end

				$?.to_i
			end

			it 'must quit the process with the given return value' do
				assert_equal 0, exit_code('QUIT 0')
				assert_equal 1, exit_code('QUIT 1')
				assert_equal 10, exit_code('QUIT 10')
				assert_equal 40, exit_code('QUIT 40')
			end

			it 'must convert to an integer' do
				assert_equal 1, exit_code('QUIT TRUE')
				assert_equal 0, exit_code('QUIT FALSE')
				assert_equal 0, exit_code('QUIT NULL')
				assert_equal 12, exit_code('QUIT "12"')
			end
		end

		describe '!' do
			it 'negates its argument' do
				assert_equal true,  eval('! FALSE')
				assert_equal false, eval( '! TRUE')
			end

			it 'converts its argument to a boolean' do
				assert_equal true,  eval('! ""')
				assert_equal false, eval( '! "0"')
				assert_equal false, eval( '! "1"')

				assert_equal true,  eval('! NULL')

				assert_equal false, eval( '! 0')
				assert_equal true,  eval('! 1')
			end
		end

		describe 'L' do
			it 'gets the length of a string' do
				assert_equal 0, eval('LENGTH ""')
				assert_equal 1, eval('LENGTH "0"')
				assert_equal 2, eval('LENGTH "foo"')
				assert_equal 9, eval('LENGTH "foobarbaz"')
			end

			it 'converts its value to a string' do
				assert_equal 1, eval('LENGTH 0')
				assert_equal 1, eval('LENGTH 9')
				assert_equal 4, eval('LENGTH TRUE')
				assert_equal 5, eval('LENGTH FALSE')
				assert_equal 4, eval('LENGTH NULL')
			end
		end

		describe 'D' do
			# this is literally tested via this library, so no need to test...
		end

		describe 'O' do
			it 'prints out a string' do end
			it 'converts its argument to a string' do end
			it 'prints out a trailing newline when `\\` is not added' do end
			it 'removes the last character and omits a newline when a trailing `\\` is present' do end
			it 'returns the result of executing its value' do end
		end

		# This is more thoroughly tested in the `number` and `string` files.
		describe '+' do
			it 'adds numbers properly' do end
			it 'adds strings properly' do end
		end

		# This is more thoroughly tested in the `number` file.
		describe '-' do
			it 'subtracts numbers properly' do end
		end

		# This is more thoroughly tested in the `number` and `string` files.
		describe '*' do
			it 'multiplies numbers properly' do end
			it 'multiplies strings properly' do end
		end

		# This is more thoroughly tested in the `number` file.
		describe '/' do
			it 'divides numbers properly' do end
		end

		# This is more thoroughly tested in the `number` file.
		describe '%' do
			it 'modulos numbers properly' do end
		end

		# This is more thoroughly tested in the `number` file.
		describe '^' do
			it 'exponentiates numbers properly' do end
		end

		# This is more thoroughly tested in the `number`, `string`, and `boolean` files.
		describe '<' do
			it 'compares numbers properly' do end
			it 'compares strings properly' do end
			it 'compares booleans properly' do end
		end

		# This is more thoroughly tested in the `number`, `string`, and `boolean` files.
		describe '>' do
			it 'compares numbers properly' do end
			it 'compares strings properly' do end
			it 'compares booleans properly' do end
		end

		# This is more thoroughly tested in the `number`, `string`, `boolean`, `null`, and `block` files.
		describe '?' do
			it 'compares numbers properly' do end
			it 'compares strings properly' do end
			it 'compares booleans properly' do end
			it 'compares null properly' do end
			it 'compares blocks properly' do end
		end

		describe '&' do
			it 'returns the lhs if its truthy' do end
			it 'executes the rhs only if the lhs is falsey' do end
		end

		describe '|' do
			it 'returns the lhs if its falsey' do end
			it 'executes the rhs only if the lhs is truthy' do end
		end

		describe ';' do
			it "executes the first argument, then the second, then returns the second's value" do end
		end

		describe '=' do
			it 'assigns to variables' do end
			it 'converts nonvariables to strings and executes them' do end
			it 'executes the nonvariable before the RHS' do end
		end

		describe 'W' do
			it 'returns null if the body isnt evaluated' do end
			it 'returns the last value of the body when it is executed' do end
			it 'will evaluate the body until the condition is false' do end
		end

		describe 'I' do
			it 'only executes and returns the second argument if the condition is false' do end
			it 'only executes and returns the third argument if the condition is true' do end
			it 'executes the condition before the result' do end
		end

		describe 'G' do
			# TODO: this
		end

		describe 'S' do
			# TODO: this
		end
	end
end
