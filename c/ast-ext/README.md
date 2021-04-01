# Knight in C
This is the AST-walker implementation of Knight in C. It's (as of April 2021), the most performant version of Knight I've developed.

# Macros
## Micro-optimizations
- `KN_ENV_NBUCKETS`: Used to adjust how many buckets the environment uses when hashing.
- `KN_ENV_CAPACITY`: Used to adjust the amount of variables that can exist in each bucket.
- `KN_STRING_PADDING_LENGTH`: Used to adjust the amount of extra padding given to embedded strings. This is generally chosen to be a number that rounds off the string's length to a multiple of two.

## Macro-optimizations
- `NDEBUG`: Disables all _internal_ debugging code. This should only be undefined when debugging.
- `KN_RECKLESS`: Assumes that absolutely no problems will occur during the execution of the program. _All_ checks for undefined behaviour are completely removed (including things like "did the `` ` `` function open properly?").
- `KN_COMPUTED_GOTOS`: Enables the use of computed gotos, which can significantly increase the speed of the parsing functions. However, since this uses nonstandard features, it's not enabled by default.

## Experiments
These are experiments I tried out out see if I could make this implementation faster. They may improve performance depending on what you're using knight for, but they don't improve the `timeit` times.
- `KN_DYNMAIC_ARGC`: Collapses consecutive functions into the same struct. That is, instead of `; a ; b ; c` being represented as `; (a) (; (b) (; (c)))`, it's represented as `; (a) (b) (c)`. This is only implemented for `;`.
- `KN_ARENA_ALLOCATE`: Allocates string _structs_ in arenas. This is only usable on systems with `mmap`. Since a decent amount of strings are embedded, this can be used to improve performance. However, I didn't see any significant changes, so this was not further explored really. (The `KN_NUM_PAGES` macro can be used to override the amount of pages.)
- `KN_STRING_CACHE`: Enable this to cache larger strings, so that their structs don't have to be allocated. Since I started using embedded strings, this has become less relevant. (The `KN_STRING_CACHE_MAXLEN` macro can be used to control the maximum length string that will be cached, and `KN_STRING_CACHE_LINESIZE` is the amount of strings per cache.)

## Extensions
- `KN_EXT_VALUE`: Enables the use of the `VALUE` function, which looks up a variable indirectly based on its argument.
- `KN_EXT_NEGATE`: Enables the use of the `~` function, which simply converts its argument to a number and negates it.
- `KN_EXT_EQL_INTERPOLATE`: Allows the use of non-identifiers on the LHS of `=`, which will be coerced to an identifier.
- `KN_EXT_CUSTOM_TYPES`
- `KN_EXT_FUNCTION`