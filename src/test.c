
/**************************************common test*****************************************/
void str_intern_test(){
	char x[] = "hello";
	char y[] = "hello";
	assert(x!=y);
	assert(str_intern(x)==str_intern(y));
	char z[] = "hello!";
	assert(str_intern(z) != str_intern(y));

}

void common_test(){
	buf_test();
	str_intern_test();
}


/**************************************lexer test*****************************************/

#define assert_token(x) assert(match_token(x))
#define assert_token_name(x) assert(token.name == str_intern(x) && match_token(TOKEN_NAME))
#define assert_token_int(x) assert(token.int_val == (x) && match_token(TOKEN_INT))
#define assert_token_float(x) assert(token.float_val == (x) && match_token(TOKEN_FLOAT))
#define assert_token_str(x) assert(strcmp(token.str_val,(x)) ==0 && match_token(TOKEN_STR))
#define assert_token_eof(x) assert(is_token(0))

void lex_test(){
	// Integer literal tests
	init_stream("2342352352");
	assert_token_int(2342352352ull);
	assert_token_eof();

	init_stream("0xffff");
	assert_token_int(0xffffull);
	assert_token_eof();

	init_stream("042");
	assert_token_int(042);
	assert_token_eof();

	init_stream("0b0101");
	assert_token_int(0b0101);
	assert_token_eof();

	init_stream("0 2342352352 0xffff 042 0b1111");
	assert_token_int(0);
	assert_token_int(2342352352ull);
	assert_token_int(0xffffull);
	assert_token_int(042);
	assert_token_int(0xf);
	assert_token_eof();

	//Float literal tests
	init_stream("3.14");
	assert_token_float(3.14);
	assert_token_eof();

	init_stream(".14");
	assert_token_float(.14);
	assert_token_eof();

	init_stream("3.14 .14 43.22     12. 3e10");
	assert_token_float(3.14);
	assert_token_float(0.14);
	assert_token_float(43.22);
	assert_token_float(12);
	assert_token_float(3e10);
	assert_token_eof();

	//Char literal tests
	init_stream("'a'");
	assert_token_int('a');
	assert_token_eof();
	init_stream("'\\n'");
	assert_token_int('\n');
	assert_token_eof();
	init_stream("'a' '\\n' '\\r'");
	assert_token_int('a');
	assert_token_int('\n');
	assert_token_int('\r');
	assert_token_eof();

	//String literal tests
	init_stream("\"foo\"");
	assert_token_str("foo");
	assert_token_eof();
	init_stream("\"a\\nb\"");
	assert_token_str("a\nb");
	assert_token_eof();

	//Operator  tests
	init_stream("+ : := ++ -- += -= < > <= << >> >= <<= >>=");
	assert_token('+');
	assert_token(':');
	assert_token(TOKEN_COLON_ASSIGN);
	assert_token(TOKEN_INC);
	assert_token(TOKEN_DEC);
	assert_token(TOKEN_ADD_ASSIGN);
	assert_token(TOKEN_SUB_ASSIGN);
	assert_token('<');
	assert_token('>');
	assert_token(TOKEN_LTEQ);
	assert_token(TOKEN_LSHIFT);
	assert_token(TOKEN_RSHIFT);
	assert_token(TOKEN_GTEQ);
	assert_token(TOKEN_LSHIFT_ASSIGN);
	assert_token(TOKEN_RSHIFT_ASSIGN);
	assert_token_eof();


	init_stream("XY+(XY)_HELLO1,234+2147");
	assert_token_name("XY");
	assert_token('+');
	assert_token('(');
	assert_token_name("XY");
	assert_token(')');
	assert_token_name("_HELLO1");
	assert_token(',');
	assert_token_int(234);
	assert_token('+');
	assert_token_int(2147);
	assert_token_eof();

}

#undef assert_token_eof
#undef assert_token_str
#undef assert_token_float
#undef assert_token_int
#undef assert_token_name
#undef assert_token

/**************************************ast test*****************************************/
void print_test(){

	Expr *exprs[] = {
		expr_int(64),
		expr_binary('+',expr_int(1),expr_int(2)),
		expr_unary('-',expr_float(3.1415926)),
		expr_ternary(expr_name("flag"),expr_str("true"),expr_str("false")),
		expr_field(expr_name("person"),"name"),
		expr_call(expr_name("fact"),(Expr*[]){expr_int(64)},1),
		expr_index(expr_field(expr_name("person"),"age"),expr_int(4)),
		expr_cast(typespec_name("int_ptr"),expr_name("void_ptr")),
		expr_cast(typespec_pointer(typespec_name("int")),expr_name("void_ptr")),
		expr_compound(typespec_name("Vector"),(Expr*[]){expr_int(1), expr_int(2)},2),
	};
	for(Expr **it = exprs; it != exprs + sizeof(exprs)/sizeof(*exprs); it++){
		print_expr_line(*it);
	}

	Stmt *stmts[] = {
		stmt_return(expr_int(12)),
		stmt_break(),
		stmt_continue(),
		stmt_block(
			(StmtBlock){
				(Stmt*[]){
					stmt_break(),
					stmt_continue(),
				},
				2,
			}
		),
		stmt_expr(expr_call(expr_name("print"),(Expr*[]){expr_int(1),expr_int(2)},2)),
		stmt_init("x", expr_int(1)),
		stmt_if(
			expr_name("flag1"),
			(StmtBlock){
				(Stmt*[]){
					stmt_return(expr_int(1)),
				},
				1,
			},
			(ElseIf[]){
				expr_name("flag2"),
				(StmtBlock){
					(Stmt*[]){
						stmt_return(expr_int(3)),
					},
					1,
				}
			},
			1,
			(StmtBlock){
				(Stmt*[]){
					stmt_return(expr_int(4)),
				},
				1,
			}
		),
		stmt_while(
			expr_name("running"),
			(StmtBlock){
				(Stmt*[]){
					stmt_assign(TOKEN_ADD_ASSIGN, expr_name("i"), expr_int(7))
				},
				1,
			}
		)
	};

	for(Stmt **it = stmts; it != stmts + sizeof(stmts)/sizeof(*stmts); it++){
		printf("\n\n");
		print_stmt(*it);
	}
}

void ast_test(){
	print_test();
}


/**************************************ast test*****************************************/
void parse_test(void) {
    const char *decls[] = {
        "var x: char[256] = {1, 2, 3, ['a'] = 4}",
        "struct Vector { x, y: float; }",
        "var v = Vector{x = 1.0, y = -1.0}",
        "var v: Vector = {1.0, -1.0}",
        "const n = sizeof(:int*[16])",
        "const n = sizeof(1+2)",
        "var x = b == 1 ? 1+2 : 3-4",
        "func fact(n: int): int { trace(\"fact\"); if (n == 0) { return 1; } else { return n * fact(n-1); } }",
        "func fact(n: int): int { p := 1; for (i := 1; i <= n; i++) { p *= i; } return p; }",
        "var foo = a ? a&b + c<<d + e*f == +u-v-w + *g/h(x,y) + -i%k[x] && m <= n*(p+q)/r : 0",
        "func f(x: int): bool { switch (x) { case 0: case 1: return true; case 2: default: return false; } }",
        "enum Color { RED = 3, GREEN, BLUE = 0 }",
        "const pi = 3.14",
        "union IntOrFloat { i: int; f: float; }",
        "typedef Vectors = Vector[1+2]",
        "func f() { do { print(42); } while(1); }",
        "typedef T = (func(int):int)[16]",
        "func f() { enum E { A, B, C } return; }",
        "func f() { if (1) { return 1; } else if (2) { return 2; } else { return 3; } }",
	};
    for (const char **it = decls; it != decls + sizeof(decls)/sizeof(*decls); it++) {
       // init_stream(NULL, *it);
        Decl *decl = parse_decl();
        printf("\n");
    }
}


