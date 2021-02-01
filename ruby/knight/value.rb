module Kb
	class Value
		include Comparable
		@types = []

		def self.inherited(cls) = @types.push(cls)
		def self.parse(stream)
			@types.lazy
				.filter_map { _1.parse stream }
				.first or raise ArgumentError, "unknown token start '#{stream[0]}'"
		end

		alias run itself

		def to_i = run.to_i
		def to_s = run.to_s
		def truthy? = run.truthy?
	end
end
