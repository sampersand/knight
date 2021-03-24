#!/bin/sh

die () { echo "$@" >&2; exit 1; }

bug () { die "bug:" "$@"; }

next_token () {
	# if we haven't already been reading a line,
	# then read the next one in.
	while true; do
		line=$(printf %s "$line" | sed 's/[][{}()[:blank:]:]*//')
		line=${line##\#*}
		[ -n "$line" ] && break
		read -r line || die "nope"
	done

	case "$(printf %c "$line")" in
		[[:digit:]])
			result=n$(expr "$line" : '\([[:digit:]]*\)')
			line=${line#${result#?}} ;;

		[[:lower:]_])
			result=i${line%%[![:lower:][:digit:]_]*}
			line=${line#${result#?}} ;;

		[\'\"])
			quote=$(printf %c "$line")
			line=${line#?}

			case $line in
				*$quote*)
					result=s$(printf %s "$line" | awk -F"$quote" '{ print $1 }')
					line=${line#*$quote} ;;
				*)
					tmp=$line
					found=
					while read -r line; do
						case $line in
							*$quote*)
								found=1
								break ;;
							*) tmp=$tmp$line ;;
						esac
					done

					if [ -z "$found" ]; then
						die "missing closing quote: $tmp$line"
					fi

					result=s$tmp$(printf %s "$line" |
						awk -F"$quote" '{ print $2 }')
					line=${line#*$quote} ;;
		esac ;;
	*)
		func=$(printf '%c' "$line")
		result=$(printf 'f%c\034' "$func")

		if printf %c "$func" | grep '[[:upper:]]' >/dev/null; then
			# if we're not punctuation, delete the remaining
			# upper chars
			line=${line#"${line%%[![:upper:]_]*}"}
		else
			# if we're punctuation, just delete that
			line=${line#"$func"}
		fi

		case "$func" in
			[NTFRP]) arity=0 ;;
			["EBCQLOD\`!"]) arity=1 ;;
			["-*+/%^?><&|;W="]) arity=2 ;;
			[GI]) arity=3 ;;
			[S]) arity=4 ;;
			*) die "unknown token start '$func'" ;;
		esac

		eval "next_token_ret_${next_token_rec=0}=\$result"

		for _ in $(seq 1 1 "$arity" 2>/dev/null); do
			next_token_rec=$((next_token_rec+1))
			next_token
			next_token_rec=$((next_token_rec-1))

			eval "next_token_ret_$next_token_rec=$(printf \
				"%s%s\034" \
				"\${next_token_ret_$next_token_rec}" \
				"\$result")"
		done

		next_token_ast=$((next_token_ast+1))
		eval "ast_token_$next_token_ast=\$next_token_ret_$next_token_rec"
		result=ast_token_$next_token_ast ;;
	esac
}

to_string () {
	if [ 0 -eq $# ]; then
		set -- "$result"
	fi
 
	case "$1" in
		[sn]*) result="${1#?}" ;;
		fT) result=true ;;
		fF) result=false ;;
		fN) result=null ;;
		*) die "cannot convert '$1' to a string." ;;
	esac
}

to_number () {
	if [ 0 -eq $# ]; then
		set -- "$result"
	fi

	case "$1" in
		n*) result=${1#?} ;;
		s*) result=$( # screw it, we're using awk lol
			awk -v n="$1" 'BEGIN {
				match(n, /^s[[:blank:]]*[-+]?[0-9]*/)
				printf "%d", int(substr(n, 2, RLENGTH)); }') ;;
		fT) result=1 ;;
		f[FN]) result=0 ;;
		*) die "cannot convert '$1' to a number." ;;
	esac
}

to_boolean () {
	if [ 0 -eq $# ]; then
		set -- "$result"
	fi

	case "$1" in
	n0|s|f[FN]) return 1 ;;
	[ns]*|fT) return 0 ;;
	*) die "cannot convert '$1' to a boolean." ;;
	esac
}

evaluate () {
	if [ 0 -eq $# ]; then
		set -- "$result"
	fi


	case "$1" in
		ast_token_*)
			: "${eval_recur=0}"
			IFS="$(printf '\034')"
			set -- $(eval echo '"${'"$1"'[*]}"')
			unset IFS
	esac


	case "$1" in
		[sn]* | f[NTF])
			result="$1" ;;

		i*)
			result="$(eval printf %s \""\$_kn_env_${1#?}"\")" ;;

		fR)
			result=n$(awk 'BEGIN { srand(); printf "%d", rand() * 4294967295; }') ;;

		fP)
			read -r
			result=s$REPLY ;;

		fE)
			evaluate "$2"
			to_string
			next_token <<EOS
$result
EOS
			evaluate ;;

		fB) 
			next_token_ast=$((next_token_ast+1))
			eval "ast_token_$next_token_ast=\$2"
			result=ast_token_$next_token_ast  ;;

		fC)
			evaluate "$2"
			result=$(eval printf %s \"\$$result\")
			evaluate ;;

		f\`)
			evaluate "$2"
			to_string
			result=s$($result) ;;

		fQ)
			evaluate "$2"
			to_number
			exit "$result" ;;

		f\!)
			evaluate "$2"

			if to_boolean; then
				result=fF
			else
				result=fT
			fi ;;

		fL)
			evaluate "$2"
			to_string
			result=n${#result} ;;

		fD)
			evaluate "$2"

			# deref identifier and function
			case "$result" in
				ast_token_*)
					IFS="$(printf '\034')"
					set -- $(eval echo '"${'"$result"'[*]}"')
					unset IFS

					# deref function
					case "$1" in
						ast_token_*)
							IFS="$(printf '\034')"
							set -- $(eval echo '"${'"$1"'[*]}"')
							unset IFS
					esac
					result="$1" ;;
			esac

			case "$result" in
				n*) printf "Number(%s)" "${result#?}" ;;
				s*) printf "String(%s)" "${result#?}" ;;
				fT) printf "Boolean(true)" ;;
				fF) printf "Boolean(false)" ;;
				fN) printf "Null()" ;;
				i*) printf "Identifier(%s)" "${result#?}" ;;
				f*) printf "Function(%s)" "${result#?}" ;;
				*) bug "unknown result '$result' encountered" ;;
			esac ;;
		fO)
			evaluate "$2"
			arg0=$result
			to_string

			if [ \\ = "$(printf %s "$result" | tail -c1)" ]; then
				printf %s "${result%?}"
			else
				printf '%s\n' "$result"
			fi

			result=$arg0 ;;

		f\+)
			evaluate "$2"

			eval "_arg0_$eval_recur=\$result"
			eval_recur=$((eval_recur+1))
			evaluate "$3"
			eval_recur=$((eval_recur-1))
			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")

			if [ s = "$(printf %c "$arg0")" ]; then
				to_string
				result="$arg0$result"
			else
				to_number
				arg1=$result
				to_number "$arg0"
				result=n$((result + arg1))
			fi ;;

		f-)
			evaluate "$2"
			to_number

			eval "_arg0_$eval_recur=\$result"
			eval_recur=$((eval_recur+1))

			evaluate "$3"
			to_number

			eval_recur=$((eval_recur-1))
			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")

			result=n$((arg0 - result)) ;;

		f\*)
			evaluate "$2"

			eval "_arg0_$eval_recur=\$result"
			eval_recur=$((eval_recur+1))

			evaluate "$3"

			eval_recur=$((eval_recur-1))
			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")

			if [ s = "$(printf %c "$arg0")" ]; then
				to_string

				if [ "$result" = 0 ]; then
					result=s
				else
					result=s$(seq -f'\000' -s "${arg0#?}" 1 "$result" | \
						sed 's/\000//')
				fi
			else
				to_number
				arg1=$result
				to_number "$arg0"
				result=n$((result * arg1))
			fi ;;

		f/)
			evaluate "$2"
			to_number

			eval "_arg0_$eval_recur=\$result"
			eval_recur=$((eval_recur+1))

			evaluate "$3"
			to_number

			eval_recur=$((eval_recur-1))
			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")

			if [ "$result" = 0 ]; then
				die "cannot divide by zero!"
			fi

			result=n$((arg0 / result)) ;;

		f%)
			evaluate "$2"
			to_number

			eval "_arg0_$eval_recur=\$result"
			eval_recur=$((eval_recur+1))

			evaluate "$3"
			to_number

			eval_recur=$((eval_recur-1))
			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")

			if [ "$result" = 0 ]; then
				die "cannot modulo by zero!"
			fi

			result=n$((arg0 % result)) ;;

		f^)
			evaluate "$2"
			to_number

			eval "_arg0_$eval_recur=\$result"
			eval_recur=$((eval_recur+1))

			evaluate "$3"
			to_number

			eval_recur=$((eval_recur-1))
			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")

			result=n$(echo "$arg0^$result" | bc) ;;

		f\?)
			evaluate "$2"

			eval "_arg0_$eval_recur=\$result"
			eval_recur=$((eval_recur+1))

			evaluate "$3"

			eval_recur=$((eval_recur-1))
			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")

			if [ "$result" = "$arg0" ]; then
				result=fT
			else
				result=fF
			fi ;;

		f\<)
			evaluate "$2"

			eval "_arg0_$eval_recur=\$result"
			eval_recur=$((eval_recur+1))

			evaluate "$3"

			eval_recur=$((eval_recur-1))
			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")
			pre=$(printf %c "$arg0")

			if [ s = "$pre" ]; then
				to_string; [ "${arg0#?}" \< "$result" ]
			elif [ n = "$pre" ]; then
				to_number; [ "${arg0#?}" -lt "$result" ]
			else
				to_boolean && [ "$arg0" = fF ]
			fi

			if [ $? = 0 ]; then
				result=fT
			else
				result=fF
			fi ;;
		f\>)
			evaluate "$2"

			eval "_arg0_$eval_recur=\$result"
			eval_recur=$((eval_recur+1))

			evaluate "$3"

			eval_recur=$((eval_recur-1))
			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")
			pre=$(printf %c "$arg0")

			# if [ "1" = "$(awk 'BEGIN{ print(a > b); }' a="$arg0" b="$result")" ]
			if [ s = "$pre" ]; then
				to_string; [ "${arg0#?}" \> "$result" ]
			elif [ n = "$pre" ]; then
				to_number; [ "${arg0#?}" -gt "$result" ]
			else
				! to_boolean && [ "$arg0" = fT ]
			fi

			if [ $? = 0 ]; then
				result=fT
			else
				result=fF
			fi ;;
		f\&)
			evaluate "$2"

			if to_boolean; then
				evaluate "$3"
			fi ;;
		f\|)
			evaluate "$2"

			if ! to_boolean; then
				evaluate "$3"
			fi ;;

		f\;)
			evaluate "$2"
			evaluate "$3" ;;

		fW)
			eval "_while_res$eval_recur=fN"

			while true; do
				eval_recur=$((eval_recur+1))
				evaluate "$2"
				eval_recur=$((eval_recur-1))

				if ! to_boolean; then
					break
				fi

				eval_recur=$((eval_recur+1))
				evaluate "$3"
				eval_recur=$((eval_recur-1))
				eval "_while_res$eval_recur=\$result"
			done

			result=$(eval printf %s "\$_while_res$eval_recur") ;;

		f=)
			if [ i = "$(printf %c "$2")" ]; then
				eval "_ident_$eval_recur=${2#?}"
			else
				evaluate "$2"
				to_string
				eval "_ident_$eval_recur=\$result"
			fi

			eval_recur=$((eval_recur+1))
			evaluate "$3"
			eval_recur=$((eval_recur-1))

			eval "_kn_env_$(eval printf %s "\$_ident_$eval_recur")=\$result" ;;

		fI)
			evaluate "$2"
			if to_boolean; then
				evaluate "$3"
			else
				evaluate "$4"
			fi ;;
		fG)
			evaluate "$2"
			to_string
			eval "_arg0_$eval_recur=\$result"

			eval_recur=$((eval_recur+1))
			evaluate "$3"
			to_number
			eval_recur=$((eval_recur-1))
			eval "_arg1_$eval_recur=\$result"

			eval_recur=$((eval_recur+1))
			evaluate "$4"
			to_number
			eval_recur=$((eval_recur-1))

			arg0="$(eval printf %s \""\$_arg0_$eval_recur\"")"
			arg1=$(eval printf %s "\$_arg1_$eval_recur")
			arg2=$result
			
			if [ 1 = "$(( ${#arg0} < (arg1 + arg2) ))" ]; then
				result=s$(printf %s "$arg0" | sed "s/.\{$arg1\}//")
			else
				result=s$(printf %s "$arg0" | \
					sed "s/.\{$arg1\}\(.\{${arg2}\}\).*/\\1/")
			fi ;;
		fS)
			evaluate "$2"
			to_string
			eval "_arg0_$eval_recur=\$result"

			eval_recur=$((eval_recur+1))
			evaluate "$3"
			to_number
			eval_recur=$((eval_recur-1))
			eval "_arg1_$eval_recur=\$result"

			eval_recur=$((eval_recur+1))
			evaluate "$4"
			to_number
			eval_recur=$((eval_recur-1))
			eval "_arg2_$eval_recur=\$result"

			eval_recur=$((eval_recur+1))
			evaluate "$5"
			to_string
			eval_recur=$((eval_recur-1))

			arg0=$(eval printf %s \"\$_arg0_$eval_recur\")
			arg1=$(eval printf %s \"\$_arg1_$eval_recur\")
			arg2=$(eval printf %s \"\$_arg2_$eval_recur\")
			arg3=$result

			if [ 1 = "$(( ${#arg0} < (arg1 + arg2) ))" ]; then
				result=s$(printf %s "$arg0" | \
					sed "s/\(.\{$arg1\}\).*/\1/")$arg3
			else
				result=s$(printf %s "$arg0" | \
					sed "s/\(.\{$arg1\}\).\{${arg2}\}/\\1$(\
						printf %s "$arg3" | sed 's|[&/\]|\\&|g'
					)/"
				)
			fi ;;
		*)
			bug "unknown function '$1' encountered!" ;;
	esac
}

if  [ 2 -ne $# ] || [ '-e' != "$1" ] && [ '-f' != "$1" ]; then
	die "usage: $0 (-e 'program' | -f filename)"
fi

if [ '-e' = "$1" ]; then
	next_token <<EOS
$2
EOS
else
	next_token <"$2"
fi

evaluate
