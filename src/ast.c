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
	union{
		//Literals
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
	StmtBlock *block;
} ElseIf;

struct Stmt{
	StmtKind kind;
	Expr *expr;
	Stmt *block;
	union {
		struct { // else if .... else
			BUF(ElseIf *elseifs);
			StmtBlock else_block;
		};
		struct {
			const char *var_name;
		};	
		struct {
			Expr *rhs;
		};
	};
};


typedef enum TypeSpecKind{
	TYPESPEC_NONE,
	TYPESPEC_PAREN,
	TYPESPEC_NAME,
	TYPESPEC_FUNC,
	TYPESPEC_ARRAY,
	TYPESPEC_POINTER,
} TypeSpecKind;

struct TypeSpec{
	TypeSpecKind kind;
};

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


