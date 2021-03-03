# Knight in C, but optimized

## FLAGS
- `NDEBUG`: disables all internal debugger checks checks.
- `RECKLESS`: assume there's no UB in source code, disables all user-code checks.
- `COMPUTED_GOTOS`: enables the use of computed gotos with some minor performance gains.
- `FIXED_ARGC`: Without this, `;` will take a variable amount of arguments.
- `KN_EXT_VALUE`: Enables the use of the `VALUE` function, which looks up a variable indirectly based on its argument.