
/**
 * expr3 = INT | '(' expr ')'
 * expr2 = [-]expr2 | exp3
 * expr1 = expr2 ([/*] expr2)* 
 * expr0 = expr1 ([+-] expr1)* 
 * expr = expr0
 */


bool is_cmp_op(){
	return (is_token(TOKEN_EQ) || 
			 is_token(TOKEN_NOTEQ) || 
			 is_token(TOKEN_LTEQ) || 
			 is_token(TOKEN_GTEQ) ||
			 is_token('<') ||
			 is_token('>'));
}

bool is_add_op(){
	return (is_token('+') ||
			 is_token('-') ||
			 is_token('|') ||
			 is_token('^'));
}

bool is_mul_op(){
	return (is_token('*') ||
			 is_token('/') ||
			 is_token('%') ||
			 is_token('&') ||
			 is_token(TOKEN_LSHIFT) ||
			 is_token(TOKEN_RSHIFT));
}

bool is_unary_op(){
	return (is_token('*') ||
			 is_token('&') ||
			 is_token('-') ||
			 is_token('+'));
}

bool is_assign_op(void) {
    return (is_token('=') ||
			 is_token(TOKEN_COLON_ASSIGN) ||
			 is_token(TOKEN_ADD_ASSIGN) ||
			 is_token(TOKEN_SUB_ASSIGN) ||
			 is_token(TOKEN_AND_ASSIGN) ||
			 is_token(TOKEN_OR_ASSIGN) ||
			 is_token(TOKEN_XOR_ASSIGN) ||
			 is_token(TOKEN_LSHIFT_ASSIGN) ||
			 is_token(TOKEN_RSHIFT_ASSIGN) ||
			 is_token(TOKEN_MUL_ASSIGN) ||
			 is_token(TOKEN_DIV_ASSIGN) ||
			 is_token(TOKEN_MOD_ASSIGN));
}

Expr *parse_expr();
TypeSpec *parse_type();
Stmt *parse_stmt();
/******************************** type praser *********************************/

TypeSpec *parse_type_func_param(){
	TypeSpec *type = parse_type();
    if (match_token(':')) {
        if (type->kind != TYPESPEC_NAME) {
            syntax_error("Colons in parameters of func types must be preceded by names.");
        }
        type = parse_type();
    }
    return type;
}

TypeSpec *parse_type_func(){
	expect_token('(');
	TypeSpec **args = NULL;
	if(!is_token(')')){
		buf_push(args,parse_type_func_param());
		while(match_token(',')){
			buf_push(args,parse_type_func_param());
		}
	}
	expect_token(')');
	TypeSpec *ret = NULL;
	if(match_token(':')){
		ret = parse_type();
	}

	return typespec_func(args,buf_len(args),ret);
}

TypeSpec *parse_type_base(){
	if(is_token(TOKEN_NAME)){
		const char *name = token.name;
		next_token();
		return typespec_name(name);
	}else if(match_keyword(func_keyword)){
		return parse_type_func();
	}else if(match_token('(')){
		return parse_type();
	}else{
		syntax_error("Unexpect token %s in type",token_kind_name(token.kind));
		return NULL;
	}
}

TypeSpec *parse_type(){
	TypeSpec *type = parse_type_base();
	while(is_token('[') || is_token('*')){
		if(match_token('[')){
			Expr *expr = NULL;
			if(!is_token(']')){
				expr = parse_expr();
			}
			expect_token(']');
			type = typespec_array(type,expr);
		}else{
			assert(is_token('*'));
			next_token();
			type = typespec_pointer(type);
		}
	}
	return type;
}

/******************************** expr praser *********************************/

Expr *parse_expr_compound(TypeSpec *type){
	printf("parse_expr_compound\n");
	expect_token('{');
	Expr **args = NULL;
	if(!is_token('}')){
		buf_push(args,parse_expr());
		while(match_token(',')){
			buf_push(args,parse_expr());
		}
	}
	expect_token('}');
	return expr_compound(type,args,buf_len(args));
}

Expr *parse_expr_operand(){
	printf("parse_expr_operand\n");
	if(is_token(TOKEN_NAME)){
		const char *name = token.name;
		next_token();
		if(is_token('{')){
			return parse_expr_compound(typespec_name(name));
		}else{
			return expr_name(name);
		}
	}else if(is_token(TOKEN_INT)){
		int64_t int_val = token.int_val;
		next_token();
		return expr_int(int_val);
	}else if (is_token(TOKEN_FLOAT)){
		double float_val = token.float_val;
		next_token();
		return expr_float(float_val);
	}else if(is_token(TOKEN_STR)){
		const char *str_val = token.str_val;
		next_token();
		return expr_str(str_val);
	}else if(is_token('{')){
		return parse_expr_compound(NULL);
	}else if(match_token('(')){
		if(is_token(':')){
			TypeSpec *type = parse_type();
			expect_token(')');
			return parse_expr_compound(type);
		}else{
			Expr *expr = parse_expr();
			expect_token(')');
			return expr;
		}
	}else{
		syntax_error("Unexpected token %s in expression",token_kind_name(token.kind));
		return NULL;
	}
}

/**
 * a(x,y)
 * a[i]
 * a.x
 */
Expr *parse_expr_base(){
	printf("parse_expr_base\n");
	Expr *expr = parse_expr_operand();
	while(is_token('(') || is_token('[') || is_token('.')){
		if(match_token('(')){
			Expr **args = NULL;
			buf_push(args,parse_expr());
			while(is_token(',')){
				buf_push(args,parse_expr());
			}
			expect_token(')');
			expr = expr_call(expr,args,buf_len(args));
		}else if(is_token('[')){
			Expr *index = parse_expr();
			expect_token(']');
			expr = expr_index(expr,index);
		}else{
			assert(is_token('.'));
			const char *field = token.name;
			next_token();
			expect_token(TOKEN_NAME);
			expr = expr_field(expr,field);
		}
	}
	return expr;
}

Expr *parse_expr_unary(){
	printf("parse_expr_unary\n");
	if(is_unary_op()){
		TokenKind op = token.kind;
		next_token();
		return expr_unary(op,parse_expr_unary());
	}else{
		return parse_expr_base();
	}
}

Expr *parse_expr_mul(){
	printf("parse_expr_mul\n");
	Expr *expr = parse_expr_unary();
	while(is_mul_op()){
		TokenKind op  = token.kind;
		next_token;
		expr = expr_binary(op,expr,parse_expr_unary());
	}
	return expr;
}

Expr *parse_expr_add(){
	printf("parse_expr_add\n");
	Expr *expr = parse_expr_mul();
	while(is_add_op()){
		TokenKind op = token.kind;
		next_token();
		expr = expr_binary(op,expr,parse_expr_mul());
	}
	return expr;
}

Expr *parse_expr_cmp(){
	printf("parse_expr_cmp\n");
	Expr *expr = parse_expr_add();
	while(is_cmp_op()){
		TokenKind op = token.kind;
		next_token();
		expr = expr_binary(op,expr,parse_expr_add());
	}
	return expr;
}

Expr *parse_expr_and(){
	printf("parse_expr_and\n");
	Expr *expr = parse_expr_cmp();
	while(match_token(TOKEN_AND)){
		expr = expr_binary(TOKEN_AND,expr,parse_expr_cmp());
	}
	return expr;
}

Expr *parse_expr_or(){
	printf("parse_expr_or\n");
	Expr *expr = parse_expr_and();
	while(match_token(TOKEN_OR)){
		expr = expr_binary(TOKEN_OR,expr,parse_expr_and());
	}

	return expr;
}

Expr *parse_expr_ternary(){
	printf("parse_expr_ternary\n");
	Expr *expr = parse_expr_or();
	if(match_token('?')){
		Expr *then_expr = parse_expr_ternary();
		expect_token(':');
		Expr* else_expr = parse_expr_ternary();
		
		expr = expr_ternary(expr,then_expr,else_expr);
	}
	return expr;
}

Expr *parse_expr(){
	printf("parse_expr\n");
	return parse_expr_ternary();
}

// ( expr )
Expr *parse_paren_expr(){
	expect_token('(');
	Expr *expr = parse_expr();
	expect_token(')');
	return expr;
}

/******************************** stmt praser *********************************/
StmtBlock parse_stmt_block();



Stmt *parse_init_stmt(Expr *left) {
    if (match_token(TOKEN_COLON_ASSIGN)) {
        if (left->kind != EXPR_NAME) {
            syntax_error(":= must be preceded by a name");
            return NULL;
        }
        return stmt_init(left->name, parse_expr());
    } else if (match_token(':')) {
        if (left->kind != EXPR_NAME) {
            syntax_error(": must be preceded by a name");
            return NULL;
        }
        const char *name = left->name;
        TypeSpec *type = parse_type();
        Expr *expr = NULL;
        if (match_token('=')) {
            expr = parse_expr();
        }
        return stmt_init(name, expr);
    } else {
        return NULL;
    }
}

Stmt *parse_simple_stmt() {
    Expr *expr = parse_expr();
    Stmt *stmt = parse_init_stmt(expr);
    if (!stmt) {
        if (is_assign_op()) {
            TokenKind op = token.kind;
            next_token();
            stmt = stmt_assign(op, expr, parse_expr());
        } else {
            stmt = stmt_expr(expr);
        }
    }
    return stmt;
}

Stmt *parse_stmt_if(){
	Expr *cond = parse_paren_expr();
	StmtBlock then_block = parse_stmt_block();
	StmtBlock else_block = {0};
	ElseIf *elseif = NULL;
	while(match_keyword(else_keyword)){
		if(!match_keyword(if_keyword)){
			else_block = parse_stmt_block();
			break;
		}
		Expr *elesif_cond = parse_paren_expr();
		StmtBlock elseif_block = parse_stmt_block();
		buf_push(elseif, ((ElseIf){elesif_cond,elseif_block}));
	}

	return stmt_if(cond,then_block,elseif,buf_len(elseif),else_block);
}

Stmt *parse_stmt_while(){
	Expr *cond = parse_paren_expr();
	StmtBlock block = parse_stmt_block();

	return stmt_while(cond,block);
}

Stmt *parse_stmt_do_while(){
	StmtBlock block = parse_stmt_block();
	match_keyword(while_keyword);
	Expr *cond = parse_paren_expr();

	return stmt_while(cond, block);
	expect_token(';');
}

Stmt *parse_stmt_for(){
	expect_token('(');
	Stmt *init = NULL;
	if(!is_token(';')){
		init = parse_simple_stmt();
	}
	expect_token(';');
	Expr *cond = NULL;
	if(!is_token(';')){
		cond = parse_expr();
	}
	Stmt *next = NULL;
	if(match_token(';')){
		if(!is_token(')')){
			next = parse_simple_stmt();
		}
	}
	expect_token(')');
	return stmt_for(init,cond,next,parse_stmt_block());
}

SwitchCase parse_stmt_switch_case(void) {
    bool is_default = false;
    bool is_first_case = true;
	Expr **exprs = NULL;
    while (is_keyword(case_keyword) || is_keyword(default_keyword)) {
        if (match_keyword(case_keyword)) {
            if (!is_first_case) {
                is_first_case = false;
            }
            buf_push(exprs, parse_expr());
            while (match_token(',')) {
                buf_push(exprs, parse_expr());
            }
        } else {
            assert(is_keyword(default_keyword));
            next_token();
            if (is_default) {
                syntax_error("Duplicate default labels in same switch clause");
            }
            is_default = true;
        }
        expect_token(':');
    }
    Stmt **stmts = NULL;
    while (!is_token_eof() && !is_token(')') && !is_keyword(case_keyword) && !is_keyword(default_keyword)) {
        buf_push(stmts, parse_stmt());
    }
    return (SwitchCase){buf_len(exprs),exprs, is_default, ((StmtBlock){stmts, buf_len(stmts)})};
}

Stmt *parse_stmt_switch(){
	Expr *expr = parse_paren_expr();
    SwitchCase *cases = NULL;
    expect_token('{');
    while (!is_token_eof() && !is_token('}')) {
        buf_push(cases, parse_stmt_switch_case());
    }
    expect_token('}');
    return stmt_switch(expr, cases, buf_len(cases));
}

StmtBlock parse_stmt_block(){
	expect_token('{');
	Stmt **stmts = NULL;
	while(!is_token_eof() && !is_token('}')){
		buf_push(stmts, parse_stmt());
	}
	expect_token('}');
	return (StmtBlock){stmts,buf_len(stmts)};
}

Stmt *parse_stmt(){
	if(match_keyword(if_keyword)){
		return parse_stmt_if();
	}else if(match_keyword(while_keyword)){
		return parse_stmt_while();
	}else if(match_keyword(for_keyword)){
		return parse_stmt_for();
	}else if(match_keyword(do_keyword)){
		return parse_stmt_do_while();
	}else if(match_keyword(switch_keyword)){
		return parse_stmt_switch();
	}else if(is_token('{')){
		return stmt_block(parse_stmt_block());
	}else if(match_keyword(return_keyword)){
		Stmt *stmt = stmt_return(parse_expr());
		expect_token(';');
		return stmt;
	}else if(match_keyword(break_keyword)){
		expect_token(';');
		return stmt_break();
	}else if(match_keyword(continue_keyword)){
		expect_token(';');
		return stmt_continue();
	}else{
		Stmt *stmt = parse_simple_stmt();
		expect_token(';');
		return stmt;
	}
}




/******************************** decl praser *********************************/

Decl *parse_decl_enum(){

}

Decl *parse_decl_aggregate(DeclKind kind){

}

const char *parse_name(){
	const char *name = token.name;
    expect_token(TOKEN_NAME);
    return name;
}

Decl *parse_decl_const(){
	const char *name = parse_name();
	expect_token('=');
	return decl_const(name, parse_expr());
}

Decl *parse_decl_typedef(){

}

Decl *parse_decl_func(){

}

Decl *parse_decl_var(){
	const char *name = parse_name();

    if (match_token('=')) {
        Expr *expr = parse_expr();
        expect_token(';');
        return decl_var(name, NULL, expr);
    } else if (match_token(':')) {
        TypeSpec *type = parse_type();
        Expr *expr = NULL;
        if (match_token('=')) {
            expr = parse_expr();
        }
        expect_token(';');
        return decl_var(name, type, expr);
    } else {
        syntax_error("Expected : or = after var, got %s", token.name);
        return NULL;
    }
}

Decl *parse_decl_import(){

}

Decl *parse_decl_note(){

}

 Decl *parse_decl_opt() {
	if (match_keyword(enum_keyword)) {
        return parse_decl_enum();

    } else if (match_keyword(struct_keyword)) {
        return parse_decl_aggregate(DECL_STRUCT);

    } else if (match_keyword(union_keyword)) {
        return parse_decl_aggregate(DECL_UNION);

    } else if (match_keyword(const_keyword)) {
        return parse_decl_const();

    } else if (match_keyword(typedef_keyword)) {
        return parse_decl_typedef();

    } else if (match_keyword(func_keyword)) {
        return parse_decl_func();

    } else if (match_keyword(var_keyword)) {
        return parse_decl_var();

    } else if (match_keyword(import_keyword)) {
        return parse_decl_import();

    } else if (match_token(TOKEN_POUND)) {
        return parse_decl_note();

    } else {
         return NULL;
    }
 }

Decl *parse_decl(void){
	Decl *decl = parse_decl_opt();

	if(!decl){
		syntax_error("Expected declaration keyword, got %s",token.name);
	}

	return decl;
}


