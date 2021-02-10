require 'minitest'
require 'minitest/autorun'
require 'minitest/spec'

require_relative '../lib/knight'

describe Kn::Text do
	EMPTY = ''
	ZERO = '0'
	ONE = '1'
	TWENTY_A = '20a'
	WITH_SPACES = 'hi there friend'
	WITH_SINGLE = "50hi ' quote"
	WITH_DOUBLE = 'hi " quote'

	before do
		@empty = Kn::Text.new EMPTY
		@zero = Kn::Text.new ZERO
		@one = Kn::Text.new ONE
		@twenty_a = Kn::Text.new TWENTY_A
		@with_spaces = Kn::Text.new WITH_SPACES
		@with_single = Kn::Text.new WITH_SINGLE
		@with_double = Kn::Text.new WITH_DOUBLE
	end

	describe 'parsing' do
		def assert_parse(input, success, equal_to=nil)
			stream = input.dup
			result = Kn::Text.parse(stream)

			if success
				fail if equal_to.nil? # sanity check
				_(result).must_equal equal_to
				_(stream).must_equal success
			else
				_(result).must_be_nil
				_(stream).must_equal input
			end
		end

		it 'will parse single quotes correctly' do
			[@empty, @zero, @one, @twenty_a, @with_spaces, @with_double].each do |input|
				assert_parse %('#{input}'), '', input
			end

			[@empty, @zero, @one, @twenty_a, @with_spaces, @with_single].each do |input|
				assert_parse %("#{input}"), '', input
			end
		end

		it 'will preserve trailing characters' do
			assert_parse %(''foo), 'foo', @empty
			assert_parse %('#@with_spaces'foo), 'foo', @with_spaces
		end

		it 'will raise a parse error on an unterimnated quote' do
			_ { Kn::Text.parse %('hey i am unterminated!) }.must_raise Kn::ParseError
			_ { Kn::Text.parse %("hey i am unterminated!) }.must_raise Kn::ParseError
		end

		it 'must not accept leading whitespace' do
			assert_parse %( "hi"), false
		end

		it 'must not accept the grave symbol' do
			assert_parse '`ls', false
			assert_parse '`ls`', false
		end
	end

	describe 'instance methods' do
		it 'is truthy when nonempty' do
			_(@empty.truthy?).must_equal false
			_(@zero.truthy?).must_equal true
			_(@one.truthy?).must_equal true
			_(@twenty_a.truthy?).must_equal true
			_(@with_spaces.truthy?).must_equal true
			_(@with_single.truthy?).must_equal true
			_(@with_double.truthy?).must_equal true
		end

		it 'ignores trailing nonumeric characters for to_i' do
			_(@empty.to_i).must_equal 0
			_(@zero.to_i).must_equal 0
			_(@one.to_i).must_equal 1
			_(@twenty_a.to_i).must_equal 20
			_(@with_spaces.to_i).must_equal 0
			_(@with_single.to_i).must_equal 50
			_(@with_double.to_i).must_equal 0
		end

		it 'returns the string for to_s' do
			_(@empty.to_s).must_equal EMPTY
			_(@zero.to_s).must_equal ZERO
			_(@one.to_s).must_equal ONE
			_(@twenty_a.to_s).must_equal TWENTY_A
			_(@with_spaces.to_s).must_equal WITH_SPACES
			_(@with_single.to_s).must_equal WITH_SINGLE
			_(@with_double.to_s).must_equal WITH_DOUBLE
		end

		it 'must only equal itself' do
			all = [@empty, @zero, @one, @twenty_a, @with_spaces, @with_single, @with_double]

			all.product all do |lhs, rhs|
				_(lhs == rhs).must_equal lhs.equal?(rhs)
			end
		end

		it 'can be compared' do
			fail 'todo comparison'
		end
	end
end
