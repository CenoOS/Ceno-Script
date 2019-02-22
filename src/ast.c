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

TypeSpec *typespec_func(FuncTypeSpec func){
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



Expr *expr_alloc(ExprKind kind){
	Expr *expr = xcalloc(1,sizeof(Expr));
	expr->kind = kind;
	return expr;
}

Expr *expr_int(uint64_t int_val){
	Expr *expr = expr_alloc(EXPR_INT);
	expr->int_val = int_val;
	return expr;
}

Expr *expr_float(double float_val){
	Expr *expr = expr_alloc(EXPR_FLOAT);
	expr->float_val = float_val;
	return expr;
}

Expr *expr_str(const char* str_val){
	Expr *expr = expr_alloc(EXPR_STR);
	expr->str_val = str_val;
	return expr;
}

Expr *expr_name(const char  *name){
	Expr *expr = expr_alloc(EXPR_NAME);
	expr->name = name;
	return expr;
}

Expr *expr_cast(TypeSpec* type, Expr *expr){
	Expr *new_expr = expr_alloc(EXPR_CAST);
	new_expr->cast.type = type;
	new_expr->cast.expr = expr;
	return new_expr;
}

Expr *expr_call(Expr *operand,size_t num_args, Expr **args){
	Expr *expr = expr_alloc(EXPR_CALL);
	expr->compound.type = operand;
	expr->compound.num_args =num_args;
	expr->compound.args = args;
	return expr;
}

Expr *expr_index(Expr *operand, Expr **index){
	Expr *expr = expr_alloc(EXPR_INDEX);
	expr->index.expr = operand;
	expr->index.index = index;
	return expr;
}

Expr *expr_field(Expr *operand, const char  *field){
	Expr *expr = expr_alloc(EXPR_FIELD);
	expr->field.expr = operand;
	expr->field.name = field;
	return expr;
}

Expr *expr_unary(TokenKind op, Expr *expr){
	Expr *new_expr = expr_alloc(EXPR_UNARY);
	new_expr->op = op;
	new_expr->unary.operand = expr;
	return new_expr;
}

Expr *expr_binary(TokenKind op, Expr *left, Expr* right){
	Expr *new_expr = expr_alloc(EXPR_BINARY);
	new_expr->op = op;
	new_expr->binary.left = left;
	new_expr->binary.right = right;
	return new_expr;
}

Expr *expr_ternary(Expr *cond,	Expr *then_expr, Expr *else_expr){
	Expr *new_expr = expr_alloc(EXPR_TERNARY);
	new_expr->ternary.cond = cond;
	new_expr->ternary.then_expr = then_expr;
	new_expr->ternary.else_expr = else_expr;
	return new_expr;
}

void print_type(TypeSpec *type){
	switch(type->kind){
		case TYPESPEC_NAME:
			printf("%s", type->name);
			break;
		case TYPESPEC_FUNC:{
			FuncTypeSpec func = type->func;
			printf("(func ");
			for(TypeSpec **it = func.args; it!=func.args+func.num_args; it++){
				printf(" ");
				print_type(*it);
			}
			printf(") ");
			print_type(func.ret);
			printf(")");
			break;
		}
		case TYPESPEC_ARRAY:
			printf("(array ");
			print_type(type->array.elem);
			printf(" ");
			print_type(type->array.size);
			printf(")");
			break;
		case TYPESPEC_POINTER:
			printf("(ptr ");
			print_type(type->ptr.elem);
			printf(")");
			break;
		default:
			assert(0);
			break;
	}
}
void print_expr(Expr* expr){
	switch(expr->kind){
		case EXPR_INT:
			printf("%" PRIu64, expr->int_val);
			break;
		case EXPR_FLOAT:
			printf("%f", expr->float_val);
			break;
		case EXPR_STR:
			printf("\"%s\"", expr->str_val);
			break;
		case EXPR_NAME:
			printf("%s", expr->name);
			break;
		case EXPR_CAST:
			printf("(cast ");
			print_type(expr->cast.type);
			printf(" ");
			print_expr(expr->cast.expr);
			printf(")");
			break;
		case EXPR_CALL:{
			printf("(");
			print_expr(expr->call.expr);
			for(Expr **it = expr->call.args; it!=expr->call.args+expr->call.num_args; it++){
				printf(" ");
				print_expr(*it);
			}
			printf(")");
			break;
		}
		case EXPR_INDEX:
			printf("(index ");
			print_expr(expr->index.expr);
			printf(" ");
			print_expr(expr->index.index);
			printf(")");
			break;
		case EXPR_FIELD:
			printf("(field ");
			print_expr(expr->field.expr);
			printf(" %s)",expr->field.name);
			break;
		case EXPR_BINARY:
			printf("(%c ",expr->op);
			print_expr(expr->binary.left);
			printf(" ");
			print_expr(expr->binary.right);
			printf(")");
			break;
		case EXPR_COMPOUND:
			printf("compound ...");
			break;
		case EXPR_UNARY:
			printf("(%c",expr->op);
			print_expr(expr->unary.operand);
			printf(")");
			break;
		case EXPR_TERNARY:
			printf("(if ");
			print_expr(expr->ternary.cond);
			printf(" ");
			print_expr(expr->ternary.then_expr);
			printf(" ");
			print_expr(expr->ternary.else_expr);
			printf(")");
			break;
		default:
			//assert(0);
			break;
	}
}

void print_expr_line(Expr *expr){
	print_expr(expr);
	printf("\n");
}

void expr_test(){

	Expr *exprs[] = {
		expr_int(64),
		expr_binary('+',expr_int(1),expr_int(2)),
		expr_unary('-',expr_float(3.1415926)),
		expr_ternary(expr_name("flag"),expr_str("true"),expr_str("false")),
		expr_field(expr_name("person"),"name"),
		expr_call(expr_name("fact"),1,&(Expr*[]){expr_int(64)}),
		expr_index(expr_field(expr_name("person"),"age"),expr_int(4)),
		expr_cast(typespec_name("int_ptr"),expr_name("void_ptr")),
		expr_cast(typespec_pointer(typespec_name("int")),expr_name("void_ptr")),
	};
	for(Expr **it = exprs; it != exprs + sizeof(exprs)/sizeof(*exprs); it++){
		print_expr_line(*it);
	}	
}

void ast_test(){
	expr_test();
}