

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

typedef struct  CompoundExpr{	// Compound literals
	TypeSpec *type;
	size_t num_args;
	Expr **args;
} CompoundExpr;

typedef struct CastExpr{ // Cast
	TypeSpec *type;
	Expr *expr;
} CastExpr;

typedef struct UnaryExpr{ // Unary
	Expr *operand;
} UnaryExpr;


typedef	struct BinaryExpr{ // Binary
	Expr *left;
	Expr *right;
} BinaryExpr;

typedef	struct TernaryExpr{ //Ternary
	Expr *cond;
	Expr *then_expr;
	Expr *else_expr;
} TernaryExpr;

typedef struct CallExpr { // Call
	Expr *expr;
	size_t num_args;
	Expr **args;
}CallExpr;

typedef struct IndexExpr {// index
	Expr *expr;
	Expr *index;
} IndexExpr;

typedef struct FieldExpr {//field
	Expr *expr;
	const char *name;
} FieldExpr;
	
struct Expr{
	ExprKind kind;
	TokenKind op;
	union{
		//Literals / names
		uint64_t int_val;
		double float_val;
		const char *str_val;
		const char *name;
		CompoundExpr compound;
		CastExpr cast;
		UnaryExpr unary;
		BinaryExpr binary;
		TernaryExpr  ternary;
		CallExpr call;
		IndexExpr index;
		FieldExpr field;
	};
};

typedef enum TypeSpecKind {
	TYPESPEC_NONE,
	TYPESPEC_NAME,
	TYPESPEC_FUNC,
	TYPESPEC_ARRAY,
	TYPESPEC_POINTER,
} TypeSpecKind;

typedef struct FuncTypeSpec {
	size_t num_args;
	TypeSpec **args;
	TypeSpec *ret;
} FuncTypeSpec;

typedef struct ArrayTypeSpec {
	TypeSpec *elem;
	Expr *size;
} ArrayTypeSpec;

typedef struct PrtTypeSpec {
	TypeSpec *elem;
} PrtTypeSpec;

struct TypeSpec {
	TypeSpecKind kind;
	struct {
		const char *name;
		FuncTypeSpec func;
		ArrayTypeSpec array;
		PrtTypeSpec ptr;
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
	STMT_DO_WHILE,
	STMT_FOR,
	STMT_SWITCH,
	STMT_ASSIGN,
	STMT_INIT,
	STMT_EXPR,
}StmtKind;

typedef struct StmtBlock {
	size_t num_stmts;
	Stmt **stmts;
} StmtBlock;

typedef struct ReturnStmt{
	Expr *expr;
}ReturnStmt;

typedef struct ElseIf {
	Expr *cond;
	StmtBlock block;
} ElseIf;

typedef struct IfStmt{ // else if .... else
	Expr *cond;
	StmtBlock then_block;
	ElseIf *elseifs;
	size_t num_elseifs;
	StmtBlock else_block;
} IfStmt;

typedef struct WhileStmt{
	Expr *cond;
	StmtBlock *block;
} WhileStmt;


typedef	struct ForStmt{ // for
	Stmt *init;
	Expr *cond;
	Stmt *next;
	StmtBlock block;
}ForStmt;


typedef struct SwitchCase {
	size_t num_exprs;
	Expr **exprs;
	bool is_default;
	StmtBlock block;
} SwitchCase;

typedef struct SwitchStmt{ // case
	Expr *expr;
	size_t num_cases;
	SwitchCase *cases;
} SwitchStmt;

typedef	struct InitStmt{ // auto assign
	const char *var_name;
	Expr *expr;
}InitStmt;	

typedef struct AssignStmt{ // assign
	TokenKind op;
	Expr* left;
	Expr *right;
}AssignStmt;

struct Stmt{
	StmtKind kind;
	union {
		ReturnStmt return_stmt;
		IfStmt	if_stmt;
		WhileStmt while_stmt;
		ForStmt for_stmt;
		SwitchStmt switch_stmt;
		StmtBlock block;
		AssignStmt assign;
		InitStmt init;
		Expr *expr;
	};
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


typedef struct FuncParam{
	const char *name;
	TypeSpec *type;
} FuncParam;

typedef struct FuncDecl{
	FuncParam *params;
	size_t num_params;
	TypeSpec *return_type;
	 StmtBlock block;
} FuncDecl;

typedef struct EnumItem{
	const char *name;
	TypeSpec *type;
} EnumItem;

typedef struct EnumDecl {
	EnumItem *items;
	size_t num_items;	
} EnumDecl;

typedef struct AggregateItem{
	const char **names;
	size_t num_names;
	TypeSpec *type;
} AggregateItem;

typedef struct AggregateDecl{
	AggregateItem *aggregate_items;
	size_t num_aggregate_items;
} AggregateDecl;

typedef struct TypedefDecl{
	TypeSpec *type;
} TypedefDecl;

typedef struct VarDecl{
	TypeSpec *type;
	Expr *expr;
} VarDecl;

typedef struct ConstDecl{
	Expr *expr;
} ConstDecl;

typedef struct Decl{
	DeclKind kind;
	const char *name;
	union{
		EnumDecl enum_decl;
		AggregateDecl aggregate;
		FuncDecl func_decl;
		TypedefDecl typedef_decl;
		VarDecl var;
		ConstDecl const_decl;
	};
} Decl;

