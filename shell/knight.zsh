#!/bin/zsh

# NOTE: This version of knight is a bit outdated, as it doesn't have the string
# functions `GET`, `SET`, and `LENGTH` that were introduced later on. Since I
# already have a sh implementation, I'm not going to bother updating this one.

abort () { echo $@ >&2; exit 1; }
matches () { eval 'case "$'${2:-REPLY}'" in '$1') true;; *) false ;; esac'; }

printwhile () {
	while read -u0 -k1; do
		if matches "$1"; then
			printf %q "$REPLY"
		else
			cached=$REPLY
			break
		fi
	done

	printf '\0'
}

tokenize () {
	while {
		if [[ -n "$cached" ]]; then
			REPLY=$cached && unset cached
		else
			read -u0 -k1
		fi
	}; do
		case $REPLY in
			[$'()\ \n\t\f:']);;
			\#) read;;
			[a-z_]) printf i%s "$REPLY" && printwhile '[a-z_0-9]';;
			[0-9])  printf 0%s "$REPLY" && printwhile '[0-9]';;
			[A-Z])  printf c%s "$REPLY" && printwhile '[A-Z]';;
			\'|\")  # -> fix dumb syntax '
				printf s && printwhile "[^\\$REPLY]" && unset cached ;;

			['-+*/^&|<>!;=%']) printf 'c%s\0' "$REPLY";;
			*) abort "unknown character '$REPLY' given."
		esac
	done
}

gen_ast () {
	IFS=$'\0' read -d $'\0' || return $?
	local ast=__knight_ast_${node:=0}
	eval $ast="($(printf %q "$REPLY"))"
	let node+=1
	if [[ 'c' == "${REPLY:0:1}" ]]; then
		case "${REPLY:1}" in
			[PTFN] | PROMPT | TRUE | FALSE | NULL)
				;;
			[BCOQES!] | BLOCK | CALL | OUTPUT | QUIT | EVAL | SYSTEM)
				gen_ast && eval $ast+='("$last_ast")' ;;
			['-WR;+*/^<>&|='] | WHILE | RANDOM)
				gen_ast && eval $ast+='("$last_ast")' && gen_ast && eval $ast+='("$last_ast")' ;;
			I | IF)
				gen_ast && eval $ast+='("$last_ast")' && \
					gen_ast && eval $ast+='("$last_ast")' && \
					gen_ast && eval $ast+='("$last_ast")' ;;
			*)
				abort "unknown command '${REPLY:1}' given";;
		esac
	fi
	last_ast=$ast
}

is_truthy () { ! matches '00 | s | cF* | cN*' ${1:-last_result}; }
to_bool1 () { if [[ "$1" == 0 ]]; then echo cFALSE; else echo cTRUE; fi }

to_bool () { 
	case ${1=$last_result} in
		cT*) echo cTRUE ;;
		cF*) echo cFALSE ;;
		cN*) echo cNULL ;;
		s*) if [[ -z "${1:1}" ]]; then echo cFALSE; else echo cTRUE; fi ;;
		0*) if [[ 0 == "${1:1}" ]]; then echo cFALSE; else echo cTRUE; fi ;;
		*) abort "cannot convert '$1' to a boolean"
	esac
}

to_string () {
	case ${1=$last_result} in
		s*) echo $1 ;;
		0*) echo s${1:1} ;;
		cT*) echo strue ;;
		cF*) echo sfalse ;;
		cN*) echo snull ;;
		*) abort "cannot convert '$1' to a string"
	esac
}

to_num () {
	case ${1=$last_result} in
		0*) echo $1 ;;
		s*)
			[[ "${1:1}" =~ '[^0-9]' ]] && abort "string isn't an int: $1"
			echo 0${1:1} ;;
		cT*) echo 01 ;;
		cF* | cN*) echo 00 ;;
		*) abort "cant convert '$1' to a number."
	esac
}

evaluate () {
	IFS=$'\0' 
	set -- $(eval echo '"${'$1'[*]}"')
	unset IFS
	case ${1?} in 
		# Literals and Nullary Commands
		0* | s* | cT* | cF* | cN*)
			last_result=$1 ;;
		i*)
			last_result=${environment[${1:1}]?"unknown variable ${1:1}"} ;;
		cP*)
			read last_result && last_result=s$last_result ;;
	
		# Unary Commands
		cB*)
			last_result=${2?} ;;
		cC*)
			evaluate ${2?} && evaluate $last_result ;;
		cQ*)
			evaluate ${2?} && exit ${last_result:1} ;;
		cE*)
			evaluate ${2?} && last_result=s$(knight ${last_result:1}) ;;
		cS*)
			evaluate ${2?} && last_result=s$(eval ${last_result:1}) ;;
		c\!)
			evaluate ${2?} && if is_truthy; then last_result=cTRUE; else last_result=cFALSE; fi;;
		cO*)
			evaluate ${2?} || return $?
			arg=$(to_string)
			if [[ \\ == ${arg:${#arg}-1} ]]; then
				printf %s ${arg:1:${#arg}-2}
			else
				printf '%s\n' ${arg:1}
			fi ;;

		# Binary Commands
		c\;)
			evaluate ${2?} && evaluate ${3?} ;;
		c=)
			local ident=$(eval echo \$${2?}) || return $?
			evaluate ${3?} || return $?
			environment[${ident:1}]=$last_result ;;
		cR*)
			evaluate ${2?} && local min=$last_result || return $?
			evaluate ${3?} && local max=$last_result || return $?
			last_result=0$(($min + $RANDOM % $max)) ;;
		cW*)
			local toreturn=cNULL
			while true; do
				evaluate ${2?} || return $?;
				if ! is_truthy; then break; fi
				evaluate $3 && toreturn=$last_result || return $?
			done
			last_result=$toreturn ;;
		c['-+*/^<>&|'])
			evaluate ${2?} && local lhs=$last_result || return $?
			evaluate ${3?} && local rhs=$last_result || return $?
			local op=${1:1}
			case $op in
				['-*/^'])
					last_result=0$(( $(to_num $lhs) ${op/^/**} $(to_num $rhs) ));;
				\+) 
					if [[ ${lhs:0:1} == 's' ]]; then
						last_result=s"${lhs:1}${rhs:1}"
					else
						last_result=0$(( $(to_num $lhs) + $(to_num $rhs) ))
					fi ;;
				['&|'])
					case $lhs in
						s*)
							local op=${op/&/-a}
							local op=${op/|/-o}
							local rhs=$(to_string $rhs)
							eval '[[ (-n "${lhs:1}") '$op' (-n "${rhs:1}") ]]'
							last_result=$(to_bool1 $(echo $?)) ;;
						0*)
							local rhs=$(to_num $rhs)
							last_result=$(to_bool1 $(( ${lhs:1} $op$op ${rhs:1} ))) ;;
						cT* | cF*)
							local rhs=$(to_bool $rhs)
							! eval '[[ "${lhs:1:1}" == "T" ]] '$op$op' [[ "${rhs:1:1}" == "T" ]]'
							last_result=$(to_bool1 $(echo $?)) ;;
						*)
							abort "unknown lhs for '$op': '$lhs'" ;;
					esac 
					;;
				['<>'])
					if [[ "${lhs:0:1}" == 's' ]]; then
						local rhs=$(to_string $rhs)
						eval '[[ "${lhs:1}" '$op' "${rhs:1}" ]]'
						last_result=$(to_bool1 $(echo $?))
					else
						local lhs=$(to_num $lhs)
						local rhs=$(to_num $rhs)
						last_result=$(to_bool1 $(( ${lhs:1} $op ${rhs:1} )))
					fi
				 ;;
				*) abort "unknown op '$op'" ;;
			esac

			;;
		# Ternary Operators
		cI*)
			evaluate $2 || return $?
			if is_truthy; then
				evaluate $3
			else
				evaluate $4
			fi ;;
		*)
			abort "unknown thing to evaluate: '$1'" ;;
	esac
}

knight () {

	echo "$1" | tokenize | gen_ast
	evaluate $last_ast
}

declare -A environment

# if [[ $# = 0 ]]; then
# 	knight "$(cat)"
# else
# 	knight $1
# fi


knight "${1:-"$(<<EOS
; = a 0
; = b 1
; = n 10
; OUTPUT (+ (+ "fib(" n) ")=\")
; WHILE (> n 1)
  ; = tmp b
  ; = b (+ b a)
  ; = a tmp
  : = n (- n 1)
: OUTPUT b
EOS
)"}"
