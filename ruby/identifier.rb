# frozen_string_literal: true
require_relative 'error'

module Kn
	class Identifier < BasicObject
		@env = {}

		def self.new(ident)
			@env[ident] ||= super
		end

		attr_reader :ident
		attr_writer :value

		def initialize(ident) = @ident = ident.freeze

		def inspect = "Identifier(#@ident)"

		def run
			@value or raise ::Kn::RunError, "uninitialized identifier '#@ident'"
		end

		# Undefine `==` it so `method_missing` will respond to it.
		undef ==

		def respond_to_missing?(...) = run.respond_to_missing?(...)
		def method_missing(...) = run.public_send(...)
	end
end
