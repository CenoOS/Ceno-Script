#include "common.c"
#include "lexer.c"

void run_test(){
	buf_test();
	lex_test();
	str_intern_test();
	parse_test();
}

int main(){
	run_test();
	return 0;
}