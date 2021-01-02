require_relative 'value'

module Kn
	class Identifier
		
	class Ast < Value
		def self.parse(stream)
			return nil if stream.empty?
			stream = stream.dup # dup & unfreeze so we don't affect the caller

			case
			when stream.slice!(/\A\d+/) then Number.new($&.to_i)
			when stream.slice!(/\A[a-z_][a-z0-9_]*/) then Kn::Identifier.new($&.to_i)
			when stream.slice!()	
		end

		def run = 1
		def clone = 1
		def inspect = 1
	end
end

__END__
/*
 * Runs an `kn_ast_t`, returning the value associated with it.
 * 
 * If any errors occur whilst running the tree, the function will abort the
 * program with a message indicating the error.
 */
struct kn_value_t kn_ast_run(const struct kn_ast_t *);

/*
 * Clones an `kn_ast_t`.
 */
struct kn_ast_t *kn_ast_clone(const struct kn_ast_t *);

/*
 * Releases all the resources the `kn_ast_t` has associated with it.
 */
void kn_ast_free(struct kn_ast_t *);

// Debugging function used to inspect an AST.
void kn_ast_dump(const struct kn_ast_t *);

#endif /* KN_AST_H */

