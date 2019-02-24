

TypeSpec *typespec_alloc(TypeSpecKind kind){
	TypeSpec *type = xcalloc(1,sizeof(TypeSpec));
	type->kind = kind;
	return type;
}

TypeSpec *typespec_name(const char* name){
	TypeSpec *type = typespec_alloc(TYPESPEC_NAME);
	type->name = name;
	return type;
} 

TypeSpec *typespec_pointer(TypeSpec *base){
	TypeSpec *type = typespec_alloc(TYPESPEC_POINTER);
	type->ptr.elem = base;
	return type;
} 

TypeSpec *typespec_func_by_funcspec(FuncTypeSpec func){
	TypeSpec *type = typespec_alloc(TYPESPEC_FUNC);
	type->func = func;
	return type;
} 

TypeSpec *typespec_array(TypeSpec *base, Expr *size){
	TypeSpec *type = typespec_alloc(TYPESPEC_ARRAY);
	type->array.elem = base;
	type->array.size = size;
	return type;
} 


TypeSpec *typespec_func(TypeSpec **args, size_t num_args, TypeSpec *ret){
	TypeSpec *type = typespec_alloc(TYPESPEC_FUNC);
	type->func.args = args;
	type->func.num_args = num_args;
	type->func.ret = ret;

	return type;
} 



Expr *expr_alloc(){
	Expr *expr = xcalloc(1,sizeof(Expr));
	return expr;
}

Expr *expr_new(ExprKind kind){
	Expr *expr = expr_alloc();
	expr->kind = kind;
	return expr;
}

Expr *expr_int(uint64_t int_val){
	Expr *expr = expr_new(EXPR_INT);
	expr->int_val = int_val;
	return expr;
}

Expr *expr_float(double float_val){
	Expr *expr = expr_new(EXPR_FLOAT);
	expr->float_val = float_val;
	return expr;
}

Expr *expr_str(const char* str_val){
	Expr *expr = expr_new(EXPR_STR);
	expr->str_val = str_val;
	return expr;
}

Expr *expr_name(const char  *name){
	Expr *expr = expr_new(EXPR_NAME);
	expr->name = name;
	return expr;
}

Expr *expr_cast(TypeSpec* type, Expr *expr){
	Expr *new_expr = expr_new(EXPR_CAST);
	new_expr->cast.type = type;
	new_expr->cast.expr = expr;
	return new_expr;
}

Expr *expr_call(Expr *operand, Expr **args, size_t num_args){
	Expr *expr = expr_new(EXPR_CALL);
	expr->compound.type = operand;
	expr->compound.num_args =num_args;
	expr->compound.args = args;
	return expr;
}

Expr *expr_index(Expr *operand, Expr **index){
	Expr *expr = expr_new(EXPR_INDEX);
	expr->index.expr = operand;
	expr->index.index = index;
	return expr;
}

Expr *expr_field(Expr *operand, const char  *field){
	Expr *expr = expr_new(EXPR_FIELD);
	expr->field.expr = operand;
	expr->field.name = field;
	return expr;
}

Expr *expr_unary(TokenKind op, Expr *expr){
	Expr *new_expr = expr_new(EXPR_UNARY);
	new_expr->op = op;
	new_expr->unary.operand = expr;
	return new_expr;
}

Expr *expr_binary(TokenKind op, Expr *left, Expr* right){
	Expr *new_expr = expr_new(EXPR_BINARY);
	new_expr->op = op;
	new_expr->binary.left = left;
	new_expr->binary.right = right;
	return new_expr;
}

Expr *expr_ternary(Expr *cond,	Expr *then_expr, Expr *else_expr){
	Expr *new_expr = expr_new(EXPR_TERNARY);
	new_expr->ternary.cond = cond;
	new_expr->ternary.then_expr = then_expr;
	new_expr->ternary.else_expr = else_expr;
	return new_expr;
}

Expr *expr_compound(TypeSpec *type, Expr **args, size_t num_args){
	Expr *new_expr = expr_new(EXPR_COMPOUND);
	new_expr->compound.type = type;
	new_expr->compound.args = args;
	new_expr->compound.num_args = num_args;
	return new_expr;
}




Decl *decl_alloc(){
	Decl* decl  = xcalloc(1,sizeof(Decl));
	return decl;
}

Decl *decl_new(DeclKind kind,const char *name){
	Decl *decl = decl_alloc();
	decl->kind = kind;
	decl->name = name;
	return decl;
}

Decl *decl_enum(const char *name,EnumItem *items, size_t num_items){
	Decl *decl = decl_new(DECL_ENUM, name);
	decl->enum_decl.items = items;
	decl->enum_decl.num_items  = num_items;
	return decl;
}

Decl *decl_aggregate(DeclKind kind, const char *name,AggregateItem *aggregate_items,size_t num_aggregate_items){
	assert(kind == DECL_STRUCT || kind == DECL_UNION);
	Decl *decl = decl_new(kind, name);
	decl->aggregate.aggregate_items = aggregate_items;
	decl->aggregate.num_aggregate_items  = num_aggregate_items;
	return decl;
}


Decl *decl_union(const char *name, AggregateItem *aggregate_items, size_t num_aggregate_items){
	Decl *decl = decl_new(DECL_UNION, name);
	decl->aggregate.aggregate_items = aggregate_items;
	decl->aggregate.num_aggregate_items  = num_aggregate_items;
	return decl;
}


Decl *decl_var(const char *name, TypeSpec *type, Expr *expr){
	Decl *decl = decl_new(DECL_VAR, name);
	decl->var.type = type;
	decl->var.expr  = expr;
	return decl;
}

Decl *decl_func(const char *name,FuncParam *params, size_t num_params, TypeSpec *return_type, StmtBlock block){
	Decl *decl = decl_new(DECL_FUNC, name);
	decl->func_decl.params = params;
	decl->func_decl.num_params  = num_params;
	decl->func_decl.return_type  = return_type;
	decl->func_decl.block  = block;
	return decl;
}


Decl *decl_const(const char *name,Expr *expr){
	Decl *decl = decl_new(DECL_CONST, name);
	decl->const_decl.expr = expr;
	return decl;
}

Decl *decl_typedef(const char *name, TypeSpec *type){
	Decl *decl = decl_new(DECL_TYPEDEF, name);
	decl->typedef_decl.type = type;
	return decl;
}


Stmt *stmt_alloc(){
	Stmt* stmt  = xcalloc(1,sizeof(Stmt));
	return stmt;
}

Stmt *stmt_new(StmtKind kind){
	Stmt *stmt = stmt_alloc();
	stmt->kind = kind;
	return stmt;
}

Stmt *stmt_return(Expr *expr){
	Stmt *stmt = stmt_new(STMT_RETURN);
	stmt->return_stmt.expr = expr;
	return stmt;
}

Stmt *stmt_break(){
	Stmt *stmt = stmt_new(STMT_BREAK);
	return stmt;
}

Stmt *stmt_continue(){
	Stmt *stmt = stmt_new(STMT_CONTINUE);
	return stmt;
}

Stmt *stmt_block(StmtBlock block){
	Stmt *stmt = stmt_new(STMT_BLOCK);
	stmt->block = block;
	return stmt;
}

Stmt *stmt_if(Expr *cond, StmtBlock then_block,	ElseIf *elseifs, size_t num_elseifs, StmtBlock else_block){
	Stmt *stmt = stmt_new(STMT_IF);
	stmt->if_stmt.cond = cond;
	stmt->if_stmt.then_block = then_block;
	stmt->if_stmt.elseifs = elseifs;
	stmt->if_stmt.num_elseifs = num_elseifs;
	stmt->if_stmt.else_block = else_block;

	return stmt;
}


Stmt *stmt_while(Expr *cond, StmtBlock block){
	Stmt *stmt = stmt_new(STMT_WHILE);
	stmt->while_stmt.cond = cond;
	stmt->while_stmt.block = block;
	return stmt;
}


Stmt *stmt_do_while(Expr *cond, StmtBlock block){
	Stmt *stmt = stmt_new(STMT_DO_WHILE);
	stmt->while_stmt.cond = cond;
	stmt->while_stmt.block = block;
	return stmt;
}


Stmt *stmt_for(Stmt *init, Expr *cond, Stmt *next, StmtBlock block){
	Stmt *stmt = stmt_new(STMT_FOR);
	stmt->for_stmt.init = init;
	stmt->for_stmt.init = init;
	stmt->for_stmt.cond = cond;
	stmt->for_stmt.next = next;
	stmt->for_stmt.block = block;
	return stmt;
}

Stmt *stmt_switch(Expr *expr, size_t num_cases, SwitchCase *cases){
	Stmt *stmt = stmt_new(STMT_SWITCH);
	stmt->switch_stmt.expr = expr;
	stmt->switch_stmt.num_cases = num_cases;
	stmt->switch_stmt.cases = cases;

	return stmt;
}

Stmt *stmt_assign(TokenKind op, Expr* left, Expr *right){
	Stmt *stmt = stmt_new(STMT_ASSIGN);
	stmt->assign.op = op;
	stmt->assign.left = left;
	stmt->assign.right = right;
	return stmt;
}

Stmt *stmt_init(const char *var_name, Expr *expr){
	Stmt *stmt = stmt_new(STMT_INIT);
	stmt->init.var_name = var_name;
	stmt->init.expr = expr;
	return stmt;
}

Stmt *stmt_expr(Expr *expr){
	Stmt *stmt = stmt_new(STMT_EXPR);
	stmt->expr = expr;
	return stmt;
}





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