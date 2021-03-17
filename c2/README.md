# Knight in C, but optimized

## FLAGS
- `NDEBUG`: disables internal debugging code.
- `KN_DYNMAIC_ARGC`: Without this, `;` will take a variable amount of arguments.

- `KN_COMPUTED_GOTOS`: enables the use of computed gotos with some minor performance gains.
- `KN_RECKLESS`: assume that the user has no undefined behaviour in their program.
- `KN_ARENA_ALLOCATE`: allocate strings in an `mmap`ed arena instead of via `malloc`.

TODO: update everything to include RECKLESS.

# Macros
## Micro-optimizations
- `KN_ENV_NBUCKETS`: Used to adjust how many buckets the environment uses when hashing.
- `KN_ENV_CAPACITY`: Used to adjust the amount of variables that can exist in each bucket.
- `KN_STRING_CACHE_MAXLEN`: 
- `KN_STRING_CACHE_LINESIZE`:

## Macro-optimizations
- `NDEBUG`: Disables all _internal_ debugging code. This should only be undefined when debugging.
- `KN_RECKLESS`: Assumes that absolutely no problems will occur during the execution of the program. _All_ checks for undefined behaviour are completely removed (including things like "did the `` ` `` function open properly?").

## Extensions
- `KN_EXT_VALUE`: Enables the use of the `VALUE` function, which looks up a variable indirectly based on its argument.
- `KN_EXT_EQL_INTERPOLATE`: Allows the use of non-identifiers on the LHS of `=`, which will be coerced to an identifier.