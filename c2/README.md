# Knight in C, but optimized

## FLAGS
- `NDEBUG`: disables all debugger checks, so any ill-formed programs are UB.
- `COMPUTED_GOTOS`: enables the use of computed gotos with some minor performance gains.
- `DYAMIC_THEN_ARGC`: Without this, `;` will take a variable amount of arguments.

- `KN_EXT_VALUE`: Enables the use of the `VALUE` function, which looks up a variable indirectly based on its argument.
- `KN_EXT_EQL_INTERPOLATE`: Allows the use of non-identifiers on the LHS of `=`, which will be coerced to an identifier.


TODO: update everything to include RECKLESS.