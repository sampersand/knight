.include "valueh.s"

.macro kn_var_run src:req, dst=\src
	mov (\src), \dst
	assert cmp, $KN_UNDEFINED, \dst, je, "unknown variable '%2$s' encountered."
.endm

.macro kn_var_name src:req, dst=\src
	mov 8(\src), \dst
	assert cmp, $0, \dst, je, "NULL variable name encountered"
.endm