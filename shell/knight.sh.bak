#!/bin/sh
#!/usr/local/bin/shellcheck

# Aborts execution after printing the message to stderr.
abort () { echo $@ >&2; exit 1; }

car () { printf %s "$1" | head -c1; }
cdr () { printf %s "$1" | tail -c $(( ${#1} - 2)); }

# The function that tokenizes all the tokens. It takes a single argument, and
# prints out tokens separated by `\0`.
tokenize () {
	# While our input isn't empty...
	while [ -n "$1" ]; do
		# Set the current character and the arguments to teh function.
		chr=$(car "$1")
		set -- "$(cdr "$1")"

		case $chr in
			# Whitespace and any type of paren are just ignored.
			["$(printf '()[]{}\ \n\t\f')"])
				continue ;;
			# Comments start with a `#` and are read to EOL
			\#)
				read -r
				continue ;;
			# Identifiers start with a lowercase letter or underscore, and contain
			# any alphanumeric + `_`s.
			[a-z_])
				printf i%s "$chr"
				while [ -n "$1" ]; do 
					case "$(car $1)" in
						[a-z_0-9]) printf %s "$(car "$1")" ;;
						*) break ;;
					esac
					set -- "$(cdr "$1")"
				done ;;
			# Integers start with any number and contain any amount of digits or `_`.
			[0-9])
				printf n%s "$chr"
				while [ -n "$1" ]; do 
					case "$(car $1)" in
						[0-9]) printf %s "$(car "$1")" ;;
						_) ;; # ignore underscores.
						*) break ;;
					esac
					set -- "$(cdr "$1")"
				done
				printf '\0'
				;;
			# # Stripn start with any number
			# [A-Z])
			# 	printf c%s "$chr"
			# 	while [ -n "$1" ]; do 
			# 		case "$(car $1)" in
			# 			[0-9]) printf %s "$(car "$1")" ;;
			# 			_) ;; # ignore underscores.
			# 			*) break ;;
			# 		esac
			# 		set -- "$(cdr "$1")"
			# 	done
			# 	printf '\0'
			# 	;;

			# [A-Z])

			\'|\")  printf s && printwhile "[^\\$chr]" && unset cached ;;
			['-+*/^&|<>!;=%']) printf 'c%s\0' "$chr";;
			*) abort "unknown character '$chr' given."
		esac

		printf '\0'
	done
}

tokenize '; = a 4'
# tokenize '; (= a 4) (OUTPUT + "a=" a)'


# # printwhile () {
# # 	while read -u0 -n1; do
# # 		if matches "$1"; then
# # 			printf %q "$REPLY"
# # 		else
# # 			cached=$REPLY
# # 			break
# # 		fi
# # 	done

# # 	printf '\0'
# # }

# # tokenize () {
# # 	while {
# # 		if [[ -n "$cached" ]]; then
# # 			chr=$cached && unset cached
# # 		else
# # 			read -u0 -n1 chr
# # 			[[ -z "$REPLY" ]]
# # 		fi
# # 	}; do
# # 		echo "'$chr'"
# # 		exit 0
# # 		case $chr in
# # 			["$(printf '()\ \n\t\f')"]) ;;
# # 			\#) read -r;;
# # 			[a-z_]) printf i%s "$chr" && printwhile '[a-z_0-9]';;
# # 			[0-9])  printf 0%s "$chr" && printwhile '[0-9]';;
# # 			[A-Z])  printf c%s "$chr" && printwhile '[A-Z]';;
# # 			\'|\")  printf s && printwhile "[^\\$chr]" && unset cached ;;
# # 			['-+*/^&|<>!;=%']) printf 'c%s\0' "$chr";;
# # 			*) abort "unknown character '$chr' given."
# # 		esac
# # 	done
# # }

# # gen_ast () {
# # 	IFS=$(printf '\0') read -d $'\0' || return $?
# # 	local ast=__knight_ast_${node:=0}
# # 	eval $ast="($(printf %q "$REPLY"))"
# # 	let node+=1
# # 	if [ c = "$(car "$REPLY")" ]; then
# # 		case "$(cdr "$REPLY")" in
# # 			[PTFN] | PROMPT | TRUE | FALSE | NULL)
# # 				;;
# # 			[BCOQES!] | BLOCK | CALL | OUTPUT | QUIT | EVAL | SYSTEM)
# # 				gen_ast && eval $ast+='("$last_ast")' ;;
# # 			['-WR;+*/^<>&|='] | WHILE | RANDOM)
# # 				gen_ast && eval $ast+='("$last_ast")' && gen_ast && eval $ast+='("$last_ast")' ;;
# # 			I | IF)
# # 				gen_ast && eval $ast+='("$last_ast")' && \
# # 					gen_ast && eval $ast+='("$last_ast")' && \
# # 					gen_ast && eval $ast+='("$last_ast")' ;;
# # 			*)
# # 				abort "unknown command '$(cdr "$REPLY")' given";;
# # 		esac
# # 	fi
# # 	last_ast=$ast
# # }

# # is_truthy () { ! matches '00 | s | cF* | cN*' last_result; }
# # to_bool1 () { if [ "$1" = 0 ]; then printf cFALSE; else printf cTRUE; fi }

# # to_bool () { 
# # 	case ${1="$last_result"} in
# # 		cT*) printf cTRUE ;;
# # 		cF*) printf cFALSE ;;
# # 		cN*) printf cNULL ;;
# # 		s*) if [ -z "$(cdr "$1")" ]; then printf cFALSE; else printf cTRUE; fi ;;
# # 		0*) if [ 0 = "$(cdr "$1")" ]; then printf cFALSE; else printf cTRUE; fi ;;
# # 		*) abort "cannot convert '$1' to a boolean"
# # 	esac
# # }

# # to_string () {
# # 	case ${1="$last_result"} in
# # 		s*) printf %s "$1" ;;
# # 		0*) printf s%s "$(cdr "$1")" ;;
# # 		cT*) printf strue ;;
# # 		cF*) printf sfalse ;;
# # 		cN*) printf snull ;;
# # 		*) abort "cannot convert '$1' to a string"
# # 	esac
# # }

# # to_num () {
# # 	case ${1="$last_result"} in
# # 		0*) printf %s "$1" ;;
# # 		s*) printf 0%d "$(cdr "$1")" || abort "string isn't an int: $1";;
# # 		cT*) printf 01 ;;
# # 		cF* | cN*) printf 00 ;;
# # 		*) abort "cant convert '$1' to a number."
# # 	esac
# # }

# # evaluate () {
# # 	local IFS=$(printf '\0')
# # 	set -- $(eval echo '"${'"$1"'[*]}"')
# # 	unset IFS
# # 	case ${1?} in 
# # 		# Literals and Nullary Commands
# # 		0* | s* | cT* | cF* | cN*)
# # 			last_result="$1" ;;
# # 		i*)
# # 			last_result="${environment[$(cdr "$1")]?"unknown variable $(cdr "$1")"}" ;;
# # 		cP*)
# # 			read last_result && last_result=s$last_result ;;
	
# # 		# Unary Commands
# # 		cB*)
# # 			last_result=${2?} ;;
# # 		cC*)
# # 			evaluate ${2?} && evaluate $last_result ;;
# # 		cQ*)
# # 			evaluate ${2?} && exit "$(cdr "$last_result")" ;;
# # 		cE*)
# # 			evaluate ${2?} && last_result=s$(knight "$(cdr "$last_result")") ;;
# # 		cS*)
# # 			evaluate ${2?} && last_result=s$(eval "$(cdr "$last_result")") ;;
# # 		c\!)
# # 			evaluate ${2?} && if is_truthy; then last_result=cTRUE; else last_result=cFALSE; fi;;
# # 		cO*)
# # 			evaluate ${2?} || return $?
# # 			arg=$(to_string)
# # 			if [[ \\ == "$(echo "$arg" | tail -c1)" ]]; then
# # 				printf %s ${arg:1:${#arg}-2}
# # 			else
# # 				printf '%s\n' "$(cdr "$arg")"
# # 			fi ;;

# # 		# Binary Commands
# # 		c\;)
# # 			evaluate ${2?} && evaluate ${3?} ;;
# # 		c=)
# # 			local ident=$(eval echo \$${2?}) || return $?
# # 			evaluate ${3?} || return $?
# # 			environment[${ident:1}]=$last_result ;;
# # 		cR*)
# # 			evaluate ${2?} && local min=$last_result || return $?
# # 			evaluate ${3?} && local max=$last_result || return $?
# # 			last_result=0$(($min + $RANDOM % $max)) ;;
# # 		cW*)
# # 			local toreturn=cNULL
# # 			while true; do
# # 				evaluate ${2?} || return $?;
# # 				if ! is_truthy; then break; fi
# # 				evaluate $3 && toreturn=$last_result || return $?
# # 			done
# # 			last_result=$toreturn ;;
# # 		c['-+*/^<>&|'])
# # 			evaluate ${2?} && local lhs=$last_result || return $?
# # 			evaluate ${3?} && local rhs=$last_result || return $?
# # 			local op=${1:1}
# # 			case $op in
# # 				['-*/^'])
# # 					last_result=0$(( $(to_num $lhs) ${op/^/**} $(to_num $rhs) ));;
# # 				\+) 
# # 					if [[ ${lhs:0:1} == 's' ]]; then
# # 						last_result=s"${lhs:1}${rhs:1}"
# # 					else
# # 						last_result=0$(( $(to_num $lhs) + $(to_num $rhs) ))
# # 					fi ;;
# # 				['&|'])
# # 					case $lhs in
# # 						s*)
# # 							local op=${op/&/-a}
# # 							local op=${op/|/-o}
# # 							local rhs=$(to_string $rhs)
# # 							eval '[[ (-n "${lhs:1}") '$op' (-n "${rhs:1}") ]]'
# # 							last_result=$(to_bool1 $(echo $?)) ;;
# # 						0*)
# # 							local rhs=$(to_num $rhs)
# # 							last_result=$(to_bool1 $(( ${lhs:1} $op$op ${rhs:1} ))) ;;
# # 						cT* | cF*)
# # 							local rhs=$(to_bool $rhs)
# # 							! eval '[[ "${lhs:1:1}" == "T" ]] '$op$op' [[ "${rhs:1:1}" == "T" ]]'
# # 							last_result=$(to_bool1 $(echo $?)) ;;
# # 						*)
# # 							abort "unknown lhs for '$op': '$lhs'" ;;
# # 					esac 
# # 					;;
# # 				['<>'])
# # 					if [[ "${lhs:0:1}" == 's' ]]; then
# # 						local rhs=$(to_string $rhs)
# # 						eval '[[ "${lhs:1}" '$op' "${rhs:1}" ]]'
# # 						last_result=$(to_bool1 $(echo $?))
# # 					else
# # 						local lhs=$(to_num $lhs)
# # 						local rhs=$(to_num $rhs)
# # 						last_result=$(to_bool1 $(( ${lhs:1} $op ${rhs:1} )))
# # 					fi
# # 				 ;;
# # 				*) abort "unknown op '$op'" ;;
# # 			esac

# # 			;;
# # 		# Ternary Operators
# # 		cI*)
# # 			evaluate $2 || return $?
# # 			if is_truthy; then
# # 				evaluate $3
# # 			else
# # 				evaluate $4
# # 			fi ;;
# # 		*)
# # 			abort "unknown thing to evaluate: '$1'" ;;
# # 	esac
# # }

# # knight () {
# # 	echo "$1" | tokenize | gen_ast
# # 	# evaluate $last_ast
# # }

# # # declare -A environment

# # knight '; (= a 4) (OUTPUT + "a=" a)'
# # # set | grep __knight_ast_
# # # exit
# # # knight '
# # # ; = a 0
# # # ; = b 1
# # # ; = n 10
# # # ; OUTPUT (+ (+ "fib(" n) ")=\")
# # # ; WHILE (> n 1)
# # #   ; = tmp b
# # #   ; = b (+ b a)
# # #   ; = a tmp
# # #   = n (- n 1)
# # # OUTPUT b
# # # '
