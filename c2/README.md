# Knight in C, but optimized

## FLAGS
- `NDEBUG`: disables internal debugging code.
- `DYAMIC_THEN_ARGC`: Without this, `;` will take a variable amount of arguments.

- `KN_COMPUTED_GOTOS`: enables the use of computed gotos with some minor performance gains.
- `KN_EXT_VALUE`: Enables the use of the `VALUE` function, which looks up a variable indirectly based on its argument.
- `KN_EXT_EQL_INTERPOLATE`: Allows the use of non-identifiers on the LHS of `=`, which will be coerced to an identifier.
- `KN_RECKLESS`: assume that the user has no undefined behaviour in their program.
- `KN_ARENA_ALLOCATE`: allocate strings in an `mmap`ed arena instead of via `malloc`.

TODO: update everything to include RECKLESS.