

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