# frozen_string_literal: true
module Kn
	class Null
		NULL = new.freeze

		def self.new = NULL

		def inspect = 'Null()'

		def to_i = 0
		def to_s = 'null'
		def truthy? = false

		alias == equal?
	end
end
