require 'minitest'
require 'minitest/spec'

describe 'String' do
	before do
		extend $current_instance
	end

	describe 'conversions' do
		it 'is falsey when empty' do
			assert_equal false, to_boolean('""')
			assert_equal true, to_boolean('"0"')
			assert_equal true, to_boolean('"1"')
			assert_equal true, to_boolean('"false"')
			assert_equal true, to_boolean('"FALSE"')
			assert_equal true, to_boolean('"hello friend"')
		end

		describe 'integer conversion' do
			it 'strips leading whitespace, and only takes digit characters' do
				assert_equal 0, to_number('""')
				assert_equal 0, to_number('"    "')
				assert_equal 0, to_number('"    x"')
				assert_equal 12, to_number('"    12x34"')
				assert_equal 14, to_number("\"    14\n56\"")
				assert_equal 0, to_number('"    0"')
				assert_equal 0, to_number('"  0000"')
				assert_equal 2349, to_number('"  002349"')
			end

			it 'accepts leading negative and positive signs' do
				assert_equal -34, to_number('"-34"')
				assert_equal -1123, to_number('"  -1123invalid"')
				assert_equal 34, to_number('"+34"')
				assert_equal 1123, to_number('"  +1123invalid"')
			end

			it 'does not interpret `0x...` and other bases' do
				assert_equal 0, to_number('"0x12"')
				assert_equal 0, to_number('" 0x12"')
				assert_equal 0, to_number('"0xag"')
				assert_equal 0, to_number('" 0xag"')

				assert_equal 0, to_number('"0b1101011"')
				assert_equal 0, to_number('" 0b1101011"')
				assert_equal 0, to_number('"0b1102011"')
				assert_equal 0, to_number('" 0b1102011"')

				assert_equal 0, to_number('"0o127"')
				assert_equal 0, to_number('" 0o127"')
				assert_equal 0, to_number('"0o129"')
				assert_equal 0, to_number('" 0o129"')

				assert_equal 127, to_number('"0127"')
				assert_equal 127, to_number('" 0127"')
				assert_equal 129, to_number('"0129"')
				assert_equal 129, to_number('" 0129"')
			end

			it 'ignores decimal and scientific notation' do
				assert_equal 12, to_number('"12.34"')
				assert_equal -12, to_number('"-12.34"')
				assert_equal 12, to_number('"12e4"')
				assert_equal 12, to_number('"12E4"')
			end
		end

		# note that strings are only allowed to contain printable characters.
		describe 'string conversion' do
			it 'is returns its contents, without escaping, when converting to a string' do
				assert_equal "a\nb", to_string("'a\nb'")
				assert_equal '12\x34', to_string('"12\x34"')
				assert_equal '12\\',  to_string('"12\"') # trailing `\` is retained.
				assert_equal '12\\\\',  to_string('"12\\\\"') # '12\\' == '12\\'
			end

			it 'does not convert `\r\n` to `\n`' do
				assert_equal "12\r\na\r\nb", to_string("'12\r\na\r\nb'")
			end

			it 'retains quotes' do
				assert_equal %('"), to_string(%(+ "'" '"'))
				assert_equal %q(\'\"), to_string(%q(+ "\'" '\"'))
			end

			it 'converts normal stuff' do # sanity check
				assert_equal "ello, world", to_string('"ello, world"')
				assert_equal "ello\t\n\r\f\vworld", to_string(%("ello\t\n\r\f\vworld"))
				assert_equal "a man a plan a canal panama", to_string("'a man a plan a canal panama'")
			end
		end

	end

	describe 'parsing' do
		# TODO
	end

	describe 'operators' do
		describe '+' do
			it 'concatenates' do
				assert_equal "1121a3", eval('+ "112" "1a3"')
				assert_equal "Plato Aristotle", eval('+ "Plato" " Aristotle"')
				assert_equal "Because why not?", eval('++ "Because " "why" " not?"')
			end

			it 'coerces to a string' do
			end
		end

		describe '*' do
			it 'duplicates itself' do
			end

			it 'coerces the RHS to a number' do
			end
		end

		describe '?' do
			it 'is only equal to itself' do
			end

			it 'is not equal to equivalent types' do
			end
		end

		describe '<' do
			it 'performs lexicographical comparison' do
			end

			it 'coerces the RHS to a number' do
			end
		end

		describe '>' do
			it 'performs lexicographical comparison' do
			end

			it 'coerces the RHS to a number' do
			end
		end
	end
end
