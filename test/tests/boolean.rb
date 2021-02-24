require 'minitest'
require 'minitest/spec'

describe 'Booleans' do
	before do
		extend $current_instance
	end

	describe 'conversions' do
		it 'is true when TRUE and false when FALSE' do
			assert_equal true, to_boolean('TRUE')
			assert_equal false, to_boolean('FALSE')
		end

		it 'is 1 when TRUE and 0 when FALSE' do
			assert_equal 1, to_number('TRUE')
			assert_equal 0, to_number('FALSE')
		end

		it 'is "true" when TRUE and "false" when FALSE' do
			assert_equal 'true', to_string('TRUE')
			assert_equal 'false', to_string('FALSE')
		end
	end

	describe 'parsing' do
		it 'parses a simple `T` or `F` properly' do
			assert_equal true, eval('T')
			assert_equal false, eval('F')
		end

		it 'does not parse `true` or `false`' do
			# when its not in scope, it fails.
			assert_fails { eval('true') }
			assert_fails { eval('false') }

			# when it is in scope, its evaluates to what its assigned.
			assert_equal 12, eval('; = true 12 : true')
			assert_equal 12, eval('; = false 12 : false')
		end

		it 'strips trailing keywords properly' do
			assert_equal 12, eval(';T12')
			assert_equal 12, eval(';TR12')
			assert_equal 12, eval(';TRU12')
			assert_equal 12, eval(';TRUE12')
			assert_equal 12, eval(';F12')
			assert_equal 12, eval(';FA12')
			assert_equal 12, eval(';FAL12')
			assert_equal 12, eval(';FALS12')
			assert_equal 12, eval(';FALSE12')
		end
	end

	describe 'operators' do
		describe '?' do
			it 'only is equal to itself' do
				assert_equal true, eval('? TRUE TRUE')
				assert_equal true, eval('? FALSE FALSE')
			end

			it 'is not equal to anything else' do
				assert_equal false, eval('? TRUE 1')
				assert_equal false, eval('? TRUE "1"')
				assert_equal false, eval('? TRUE "TRUE"')
				assert_equal false, eval('? TRUE "true"')

				assert_equal false, eval('? FALSE 0')
				assert_equal false, eval('? FALSE ""')
				assert_equal false, eval('? FALSE "0"')
				assert_equal false, eval('? FALSE "FALSE"')
				assert_equal false, eval('? FALSE "false"')
			end
		end

		describe '<' do
			it 'is only true when FALSE and the rhs is truthy' do
				assert_equal true, eval('< FALSE TRUE')
				assert_equal true, eval('< FALSE 1')
				assert_equal true, eval('< FALSE "1"')
			end

			it 'is false all other times' do
				assert_equal false, eval('< FALSE FALSE')
				assert_equal false, eval('< FALSE 0')
				assert_equal false, eval('< FALSE ""')
				assert_equal false, eval('< FALSE NULL')

				assert_equal false, eval('< TRUE TRUE')
				assert_equal false, eval('< TRUE FALSE')
				assert_equal false, eval('< TRUE 1')
				assert_equal false, eval('< TRUE "1"')
				assert_equal false, eval('< TRUE 0')
				assert_equal false, eval('< TRUE ""')
				assert_equal false, eval('< TRUE NULL')
			end
		end

		describe '>' do
			it 'is only true when TRUTHY and the rhs is falsey' do
				assert_equal true, eval('> TRUE FALSE')
				assert_equal true, eval('> TRUE 0')
				assert_equal true, eval('> TRUE ""')
				assert_equal true, eval('> TRUE NULL')
			end

			it 'is false all other times' do
				assert_equal false, eval('> TRUE TRUE')
				assert_equal false, eval('> TRUE 1')
				assert_equal false, eval('> TRUE "1"')

				assert_equal false, eval('> FALSE TRUE')
				assert_equal false, eval('> FALSE FALSE')
				assert_equal false, eval('> FALSE 1')
				assert_equal false, eval('> FALSE "1"')
				assert_equal false, eval('> FALSE 0')
				assert_equal false, eval('> FALSE ""')
				assert_equal false, eval('> FALSE NULL')
			end
		end
	end
end
