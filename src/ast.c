typedef struct Expr Expr;
typedef struct Stmt Stmt; 
typedef struct TypeSpec TypeSpec;

typedef enum ExprKind{
	EXPR_NONE,
	EXPR_INT,
	EXPR_FLOAT,
	EXPR_STR,
	EXPR_NAME,
	EXPR_COMPOUND,
	EXPR_CAST,
	EXPR_BINARY,
	EXPR_CALL,
	EXPR_INDEX,
	EXPR_FIELD,
	EXPR_UNARY,
	EXPR_TERNARY,
} ExprKind;


struct Expr{
	ExprKind kind;
	TokenKind op;
	union{
		//Literals / names
		uint64_t int_val;
		double float_val;
		const char *str_val;
		const char *name;
		struct {	// Compound literals
			TypeSpec *compound_type;
			BUF(Expr **compound_args);
		};
		struct { // Cast
			TypeSpec *cast_type;
			Expr *cast_expr;
		};
		struct { // Unary
			Expr *operand;
			struct{ // Call
				BUF(Expr **args);
				Expr *index;
				const char *field;
			};
		};
		struct { // Binary
			Expr *left;
			Expr *right;
		};
		struct{ // Ternary
			Expr *cond;
			Expr *then_expr;
			Expr *else_expr;
		};
	};
};

typedef enum TypeSpecKind{
	TYPESPEC_NONE,
	TYPESPEC_NAME,
	TYPESPEC_FUNC,
	TYPESPEC_ARRAY,
	TYPESPEC_POINTER,
} TypeSpecKind;

typedef struct FuncTypeSpec{
	BUF(TypeSpec **args);
	TypeSpec *ret;
} FuncTypeSpec;

struct TypeSpec{
	TypeSpecKind kind;
	struct {
		const char *name;
		FuncTypeSpec func;
		struct { // pointer/arr
			TypeSpec *base;
			Expr *size;
		};
	};
};


typedef enum StmtKind{
	STMT_NONE,
	STMT_RETURN,
	STMT_BREAK,
	STMT_CONTINUE,
	STMT_BLOCK,
	STMT_IF,
	STMT_WHILE,
	STMT_FOR,
	STMT_DO,
	STMT_SWITCH,
	STMT_ASSIGN,
	STMT_AUTO_ASSIGN,
	STMT_EXPR,
}StmtKind;

typedef struct StmtBlock {
	BUF(Stmt **stmts);
} StmtBlock;

typedef struct ElseIf {
	Expr *cond;
	StmtBlock block;
} ElseIf;

typedef struct Case {
	BUF(Expr **exprs);
	StmtBlock block;
} Case;

struct Stmt{
	StmtKind kind;
	// return if while do switch
	Expr *expr;
	Stmt *block;
	union {
		struct { // else if .... else
			BUF(ElseIf *elseifs);
			StmtBlock else_block;
		};
		struct { // for
			StmtBlock for_init_block;
			StmtBlock for_next_block;
		};
		struct { // case
			BUF(Case *cases);
		};
		struct { // auto assign
			const char *var_name;
		};	
		struct { // assign
			Expr *rhs;
		};
	};
};


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
	new_expr->cast_type = type;
	new_expr->cast_expr = expr;
	return new_expr;
}

Expr *expr_call(Expr *operand, Expr **args){
	Expr *expr = expr_alloc(EXPR_CALL);
	expr->operand = operand;
	expr->args = args;
	return expr;
}

Expr *expr_index(Expr *operand, Expr **index){
	Expr *expr = expr_alloc(EXPR_INDEX);
	expr->operand = operand;
	expr->index = index;
	return expr;
}

Expr *expr_field(Expr *operand, const char  *field){
	Expr *expr = expr_alloc(EXPR_FIELD);
	expr->operand = operand;
	expr->field = field;
	return expr;
}

Expr *expr_unary(TokenKind op, Expr *expr){
	Expr *new_expr = expr_alloc(EXPR_UNARY);
	new_expr->op = op;
	new_expr->operand = expr;
	return new_expr;
}

Expr *expr_binary(TokenKind op, Expr *left, Expr* right){
	Expr *new_expr = expr_alloc(EXPR_BINARY);
	new_expr->op = op;
	new_expr->left = left;
	new_expr->right = right;
	return new_expr;
}

Expr *expr_ternary(Expr *cond,	Expr *then_expr, Expr *else_expr){
	Expr *new_expr = expr_alloc(EXPR_TERNARY);
	new_expr->cond = cond;
	new_expr->then_expr = then_expr;
	new_expr->else_expr = else_expr;
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
			for(TypeSpec **it = func.args; it!=buf_end(func.args); it++){
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
			print_type(type->base);
			printf(" ");
			print_type(type->size);
			printf(")");
			break;
		case TYPESPEC_POINTER:
			printf("(ptr ");
			print_type(type->base);
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
			print_type(expr->cast_type);
			printf(" ");
			print_expr(expr->cast_expr);
			printf(")");
			break;
		case EXPR_CALL:{
			printf("(");
			print_expr(expr->operand);
			for(Expr **it = expr->args; it!=buf_end(expr->args); it++){
				printf(" ");
				print_expr(*it);
			}
			printf(")");
			break;
		}
		case EXPR_INDEX:
			printf("(index ");
			print_expr(expr->operand);
			printf(" ");
			print_expr(expr->index);
			printf(")");
			break;
		case EXPR_FIELD:
			printf("(field ");
			print_expr(expr->operand);
			printf(" %s)",expr->field);
			break;
		case EXPR_BINARY:
			printf("(%c ",expr->op);
			print_expr(expr->left);
			printf(" ");
			print_expr(expr->right);
			printf(")");
			break;
		case EXPR_COMPOUND:
			printf("compound ...");
			break;
		case EXPR_UNARY:
			printf("(%c",expr->op);
			print_expr(expr->operand);
			printf(")");
			break;
		case EXPR_TERNARY:
			printf("(if ");
			print_expr(expr->cond);
			printf(" ");
			print_expr(expr->then_expr);
			printf(" ");
			print_expr(expr->else_expr);
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
	Expr **fact_args = NULL;
	buf_push(fact_args,expr_int(64));
	Expr *exprs[] = {
		expr_int(64),
		expr_binary('+',expr_int(1),expr_int(2)),
		expr_unary('-',expr_float(3.1415926)),
		expr_ternary(expr_name("flag"),expr_str("true"),expr_str("false")),
		expr_field(expr_name("person"),"name"),
		expr_call(expr_name("face"),fact_args),
		expr_index(expr_field(expr_name("person"),"age"),expr_int(4))
	};
	for(Expr **it = exprs; it != exprs + sizeof(exprs)/sizeof(*exprs); it++){
		print_expr_line(*it);
	}	
}



typedef enum DeclKind{
	DECL_ENUM,
	DECL_STRUCT,
	DECL_UNION,
	DECL_VAR,
	DECL_CONST,
	DECL_TYPEDEF,
	DECL_FUNC,
} DeclKind;

typedef struct EnumItem{
	const char *name;
	TypeSpec *type;
} EnumItem;

typedef struct AggregateItem{
	BUF(const char **name);
	TypeSpec *type;
} AggregateItem;

typedef struct FuncParam{
	const char *name;
	TypeSpec *type;
} FuncParam;

typedef struct FuncDecl{
	BUF(FuncParam *params);
	TypeSpec *return_type;
} FuncDecl;

typedef struct Decl{
	DeclKind kind;
	const char *name;
	union{
		struct{
			BUF(EnumItem *items);
			BUF(AggregateItem *aggregate_items);
			struct{
				TypeSpec *type;
				Expr *expr;
			};
			FuncDecl func_decl;
		};
	};
} Decl;




void ast_test(){
	expr_test();
}