module Kn; module Test; end end

module Kn::Test::Harness
	KNOWN_SANITIZERS = %i(
		required_args undefined_identifiers zero_division negative_modulo
		4.3.5-negative-modulo-base
	).freeze

	KNOWN_EXTENSIONS = %i(
		universal_equality ignore_trailing_text
	).freeze

	@sanitizers = []
	@extensions = []

	class << self
		private def validate_sanitizer(sanitizer)
			warn "unknown sanitizer '#{sanitizer}'" unless KNOWN_SANITIZERS.include? sanitizer
		end

		private def validate_extension(extension)
			warn "unknown extension '#{extension}'" unless KNOWN_EXTENSIONS.include? extension
		end

		def sanitizers(*sanitizers)
			sanitizers.each do |san|
				validate_sanitizer san
			end

			@sanitizers.concat sanitizers
		end

		alias sanitizes sanitizers

		def extensions(*extensions)
			extensions.each do |ext|
				validate_extension ext
			end

			@sanitizers.concat extensions
		end

		alias extends extensions

		def sanitizes?(*sanitizers)
			sanitizers.all? { |sanitizer| @sanitizers.include? sanitizer }
		end

		def extension?(extension)
			sanitizers.all? { |sanitizer| @sanitizers.include? sanitizer }
		end

		alias extends? extension?
	end

	def describe(msg, **kw, &block)
		super(msg, &block) if should_test?(**kw)
	end

	def it(msg, **kw, &block)
		super(msg, &block) if should_test?(**kw)
	end
	
	def should_test?(sanitizes: nil, extension: nil)
		sanitizes = Array(sanitizes)
		extension = Array(extension)

		sanitizes.all? { |s| Harness.sanitize? s } &&
		extension.all? { |s| Harness.extension? s }
	end
end