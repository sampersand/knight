require 'minitest'
require 'minitest/spec'
require_relative 'shared'
require_relative '../harness'

describe 'Block' do
	include Kn::Test::Shared
	extend Kn::Test::Harness

	describe 'conversions' do
		# Blocks cannot be converted to anything
	end

	describe 'parsing' do
		it 'takes any argument type' do
			assert_runs { eval 'BLOCK 1' }
			assert_runs { eval 'BLOCK "a"' }
			assert_runs { eval 'BLOCK TRUE' }
			assert_runs { eval 'BLOCK FALSE' }
			assert_runs { eval 'BLOCK NULL' }
			assert_runs { eval 'BLOCK BLOCK 3' }
			assert_runs { eval 'BLOCK + 5 4' }
			assert_runs { eval 'BLOCK ident' }
		end

		
		it 'requires an argument', sanitizes: :required_args do
			assert_fails { eval 'BLOCK' }
			assert_fails { eval 'BLOCK BLOCK' }
		end

		it 'strips trailing keywords properly' do
			assert_runs { eval 'B1' }
			assert_runs { eval 'BL!1' }
			assert_runs { eval 'BLO!1' }
			assert_runs { eval 'BLO RANDOM' }
			assert_runs { eval 'BL_O RANDOM' }
			assert_runs { eval 'BLOCK 1' }
			assert_runs { eval 'BLOa' }
			assert_runs { eval 'BLa' }
			assert_runs { eval 'Ba' }
		end
	end

	it 'wont execute its body until called' do
		assert_runs { eval 'BLOCK QUIT 1' }
		assert_runs { eval 'BLOCK missing' }
		assert_runs { eval 'BLOCK EVAL "nope"' }
	end

	# note that `BLOCK` simply returns its argument, unevaluated. But in the case of 
	# literals, this is the same as the literal itself, so we must provide a function istead.
	describe 'operators' do
		describe 'CALL' do
			it 'executes its body' do
				assert_equal 12, eval('CALL BLOCK + 5 7')
				assert_equal 13, eval('; = foo BLOCK bar ; = bar 13 : CALL bar')
			end

			it 'can be called with any type' do
				assert_equal 1, eval('CALL BLOCK 1')
				assert_equal 'foo', eval('CALL BLOCK "foo"')
				assert_equal true, eval('CALL BLOCK TRUE')
				assert_equal false, eval('CALL BLOCK FALSE')
				assert_equal :null, eval('CALL BLOCK NULL')
				assert_equal 1, eval('; = ident 1 : CALL BLOCK ident')
				assert_equal 31, eval('CALL BLOCK + 9 12')
			end
		end

		describe '?', extension: :universal_equality do
			it 'is only equivalent to _the exact instance_' do
				assert_equal true, eval('; = x B R : ? x x')
			end

			it 'is not equal to anything else' do
				assert_equal false, eval('? B (! TRUE) B (! TRUE)')
				assert_equal false, eval('? B (! TRUE) FALSE')
			end
		end
	end
end
