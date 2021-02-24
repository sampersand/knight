require 'minitest'
require 'minitest/autorun'
require 'minitest/spec'

require_relative '../lib/knight'

describe Kn::Null do
	before do
		@null = Kn::Null.new
	end

	describe 'parsing' do
		def assert_parse(input, success)
			stream = input.dup
			result = Kn::Null.parse(stream)

			if success
				_(result).must_equal @null
				_(stream).must_equal success
			else
				_(result).must_be_nil
				_(stream).must_equal input
			end
		end

		it 'must parse as a keyword' do
			%w(N NULL NONE NIL NADA NOTHIN).each do |input|
				assert_parse input, ""
			end
		end

		it 'must not swallow "_"' do
			assert_parse 'NULL_HERE', '_HERE'
		end

		it 'must not accept leading whitespace' do
			assert_parse ' NULL', false
		end

		it 'must not accept lower-case null' do
			assert_parse 'null', false
		end

		it 'must not swallow trailing characters' do
			assert_parse 'Null', 'ull'
		end
	end

	describe 'instance methods' do
		it 'is falsey' do
			_(@null.truthy?).must_equal false
		end

		it 'is zero' do
			_(@null.to_i).must_equal 0
		end

		it 'is "null"' do
			_(@null.to_s).must_equal 'null'
		end

		it 'must only equal itself' do
			_(@null).must_equal Kn::Null.new
			_(@null).wont_equal nil
		end

		it 'cannot be compared' do
			_ { @null <=> nil }.must_raise Kn::RunError
			_ { @null <=> @null }.must_raise Kn::RunError
		end
	end
end
