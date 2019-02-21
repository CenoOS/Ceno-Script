#include "common.c"
#include "lexer.c"
#include "parser.c"

void run_test(){
	common_test();
	lex_test();
	parse_test();
}

int main(){
	run_test();
	return 0;
}