.PHONY: clean

run: knight
	./knight -e 'D 3'

knight: *.s
	gcc $^ -o knight

clean:
	-@rm knight
# {
# 	// "cmd": ["gcc", "$file", "-o", "knight"],
# 	// "shell_cmd": "gcc *.s -o knight && ./knight -e \\$'# hi\n\t 1234 abc \"im a string\" stuff'",
# 	"shell_cmd": "gcc *.s -o knight && ./knight -e '! \"a\"'",
# 	// "shell_cmd": "gcc *.s -o knight && ./knight -e \\$' # hi\n\t 1234  abc \"hi friend\" NULL OUTPUT '",
# 	// "shell_cmd": "gcc *.s -o knight && ./knight -e '; = a 3 : O + \"a*4=\" * a 4'",
# 	"file_patterns": ["*.s"],
# }
