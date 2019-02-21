/**
 * expr3 = INT | '(' expr ')'
 * expr2 = [-]expr2 | exp3
 * expr1 = expr2 ([/*] expr2)* 
 * expr0 = expr1 ([+-] expr1)* 
 * expr = expr0
 */
int parse_expr();

int parse_expr3(){
	if(is_token(TOKEN_INT)){
		uint64_t val = token.int_val;
		next_token();
		return val;
	}else if(match_token('(')){
		int val = parse_expr();
		expect_token(')');
		return val;
	}else{
		fatal("expected integer or (, got %s",token_kind_name(token.kind));
		return 0;
	}
}

int parse_expr2(){
	if(match_token('-')){
		return -parse_expr2();
	}else{
		return parse_expr3();
	}
}

int parse_expr1(){
	int val = parse_expr2();
	while(is_token('*') || is_token('/')){
		char op = token.kind;
		next_token();

		int rval =	parse_expr2();
		if(op=='*'){
			val*=rval;
		}else{
			assert(op=='/');
			assert(rval!=0);
			val/=rval;
		}
	}
	return val;
}

int parse_expr0(){
	int val = parse_expr1();
	while(is_token('+') || is_token('-')){
		char op = token.kind;
		next_token();
		int rval = 	parse_expr1();
		if(op=='+'){
			val+=rval;
		}else{
			assert(op=='-');
			val-=rval;
		}
	}
	return val;
}
int parse_expr(){
	return parse_expr0();
}


int parse_expr_str(const char *str){
	init_stream(str);
	return parse_expr();
}

#define TEST_EXPR(x) assert(parse_expr_str(#x) == (x))
void parse_test(){
	TEST_EXPR(1);
	TEST_EXPR((1));
	TEST_EXPR(1-2-3);
	TEST_EXPR(2*3+4*5);
	TEST_EXPR(-2*3+4*5);
	TEST_EXPR(-2+-4);
	TEST_EXPR(-2+(-4*7)*4);
}
#undef TEST_EXPR
