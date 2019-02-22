#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <inttypes.h>
#include "common.c"
#include "lexer.c"
#include "ast.c"
#include "parser.c"

void run_test(){
	common_test();
	lex_test();
	ast_test();
	parse_test();
}

int main(){
	run_test();
	return 0;
}