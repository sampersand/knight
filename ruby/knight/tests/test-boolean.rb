require 'minitest'
require 'minitest/autorun'
require 'minitest/spec'

require_relative '../lib/knight'

describe Kn::Boolean do
	before do
		@true = Kn::Boolean.new true
		@false = Kn::Boolean.new false
	end

	describe 'parsing' do
		def assert_parse(input, success, equal_to=nil)
			stream = input.dup
			result = Kn::Boolean.parse(stream)

			if success
				fail if equal_to.nil? # sanity check
				_(result).must_equal equal_to
				_(stream).must_equal success
			else
				_(result).must_be_nil
				_(stream).must_equal input
			end
		end

		it 'must parse as a keyword' do
			%w(T TRUE TRUTH TYUPTHISIATHING).each do |input|
				assert_parse input, "", @true
			end

			%w(F FALSE FALSEHOOD FYUPHITHEREFRIEND).each do |input|
				assert_parse input, "", @false
			end
		end

		it 'must not swallow "_"' do
			assert_parse 'TRUE_HERE', '_HERE', @true
		end

		it 'must not accept leading whitespace' do
			assert_parse ' TRUE', false
			assert_parse ' FALSE', false
		end

		it 'must not accept lower-case true or false' do
			assert_parse 'true', false
			assert_parse 'false', false
		end

		it 'must not swallow trailing characters' do
			assert_parse 'True', 'rue', @true
			assert_parse 'False', 'alse', @false
		end
	end

	describe 'instance methods' do
		it 'is truthy when true and falsey when false' do
			_(@true.truthy?).must_equal true
			_(@false.truthy?).must_equal false
		end

		it 'is 1 when true and 0 when false' do
			_(@true.to_i).must_equal 1
			_(@false.to_i).must_equal 0
		end

		it 'is "true" when true and "false" when false' do
			_(@true.to_s).must_equal 'true'
			_(@false.to_s).must_equal 'false'
		end

		it 'must only equal itself' do
			_(@true).must_equal @true
			_(@true).wont_equal @false
			_(@true).wont_equal true

			_(@false).must_equal @false
			_(@false).wont_equal @true
			_(@false).wont_equal false
		end

		it 'can be compared' do
			_(@true <=> @true).must_equal 0
			_(@false <=> @false).must_equal 0
			_(@true <=> @false).must_be :positive?
			_(@false <=> @true).must_be :negative?
		end
	end
end
