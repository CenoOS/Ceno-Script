

void print_typespec(TypeSpec *type){
	switch(type->kind){
		case TYPESPEC_NAME:
			printf("%s", type->name);
			break;
		case TYPESPEC_FUNC:{
			FuncTypeSpec func = type->func;
			printf("(func ");
			for(TypeSpec **it = func.args; it!=func.args+func.num_args; it++){
				printf(" ");
				print_typespec(*it);
			}
			printf(") ");
			print_typespec(func.ret);
			printf(")");
			break;
		}
		case TYPESPEC_ARRAY:
			printf("(array ");
			print_typespec(type->array.elem);
			printf(" ");
			print_typespec(type->array.size);
			printf(")");
			break;
		case TYPESPEC_POINTER:
			printf("(ptr ");
			print_typespec(type->ptr.elem);
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
			print_typespec(expr->cast.type);
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





void print_aggregate_item(AggregateItem item){
    print_typespec(item.type);
    for(char *it = item.names; it != item.names+item.num_names; it++){
        printf("%s ",it);
    }
}

void print_aggregate_decl(Decl *decl){
    for(AggregateItem *it = decl->aggregate.aggregate_items; it != decl->aggregate.aggregate_items + decl->aggregate.num_aggregate_items; it++){
        print_aggregate_item(*it);
    }
}


uint64_t indent = 0;
void print_newline(){
    printf("\n");
    for(int i =0 ;i<indent;i++){
        printf("  ");
    }
}


void print_decl(Decl *decl){
    Decl *d = decl;
    switch (d->kind)
    {
        case DECL_ENUM:
            printf("( enum %s",d->name);
            indent++;
            for(EnumItem *it = d->enum_decl.items; it!=d->enum_decl.items + d->enum_decl.num_items; it++){
                print_newline();
                printf("%s", it->name);
                //if(it->init){
                //    print_expr(it->init);
                //}else{
                    printf("nil");
                //}
                printf(")");
            }
            indent--;
            printf(")");
            break;
        
        case DECL_STRUCT:
            printf("(struct %s",d->name);
            indent++;
            print_aggregate_decl(d);
            indent--;
            printf(")");
            break;
    
        case DECL_UNION:
            printf("(union %s",d->name);
            indent++;
            print_aggregate_decl(d);
            indent--;
            printf(")");
            break;

        case DECL_VAR:
            printf("(var %s ",d->name);
            if(d->var.type){
                print_typespec(d->var.type);
            }else{
                printf("nil");
            }
            printf(" ");
            print_expr(d->var.expr);
            printf(")");
            break;

        case DECL_CONST:
            printf("(const %s ",d->name);
            print_expr(d->const_decl.expr);
            printf(")");

            break;

        case DECL_TYPEDEF:
            printf("(typedef %s ",d->name);
            print_typespec(d->typedef_decl.type);
            printf(")");
            break;

        case DECL_FUNC:
            printf("(func %s",d->name);
            for(FuncParam *it = d->func_decl.params; it != d->func_decl.params + d->func_decl.num_params; it++){
                print_typespec(&(*it->type));
                printf("%s",*it->name);
            }
            print_typespec(d->func_decl.return_type);
            indent++;
            print_stmt_block(d->func_decl.block);
            indent--;
            printf(")");
            break;

        default:
            break;
    }
}


void print_stmt_block(StmtBlock block){
    printf("(block");
    indent++;
    for(Stmt **it = block.stmts; it != block.stmts + block.num_stmts; it++){
        print_newline();
        print_stmt(*it);
    }
    indent--;
    printf(")");
}

void print_stmt_case(SwitchCase case_stmt){
    print_stmt_block(case_stmt.block);
    if(case_stmt.is_default){
        printf("(default ");
        print_stmt_block(case_stmt.block);
    }else{
        printf("(case");
        for(Expr *it = case_stmt.exprs; it != case_stmt.num_exprs; it++){
            print_expr(it);
        }
    }
}

void print_stmt(Stmt *stmt){
    Stmt *s = stmt;
    switch (s->kind){
        case STMT_RETURN:
            printf("(return ");
            print_expr(s->return_stmt.expr);
            printf(")");
            break;

        case STMT_BREAK:
            printf("(break)");
            break;

        case STMT_CONTINUE:
            printf("(continue)");
            break;

        case STMT_BLOCK:
            print_stmt_block(s->block);
            break;

        case STMT_IF:
            printf("(if ");
            print_expr(s->if_stmt.cond);
            indent++;
            print_newline();
            print_stmt_block(s->if_stmt.then_block);
            for(ElseIf *it = s->if_stmt.elseifs; it!=s->if_stmt.elseifs + s->if_stmt.num_elseifs; it++){
                print_newline();
                printf("elseif ");
                print_expr(it->cond);
                print_newline();
                print_stmt_block(it->block);
            }
            if(s->if_stmt.else_block.num_stmts != 0){
                print_newline();
                printf("else ");
                print_stmt_block(s->if_stmt.else_block);
            }
            indent--;
            printf(")");
            break;

        case STMT_WHILE:
            printf("(while ");
            print_expr(s->while_stmt.cond);
            indent++;
            print_newline();
            print_stmt_block(s->while_stmt.block);
            indent--;
            printf(")");
            break;

        case STMT_DO_WHILE:
            printf("(do ");
            print_expr(s->while_stmt.cond);
            indent++;
            print_newline();
            print_stmt_block(s->while_stmt.block);
            indent--;
            printf(")");
            break;

        case STMT_FOR:
            printf("(for ");
            print_stmt(s->for_stmt.init);
            print_expr(s->for_stmt.cond);
            print_stmt(s->for_stmt.next);
            indent++;
            print_newline();
            print_stmt_block(s->for_stmt.block);
            indent--;
            printf(")");
            break;

        case STMT_SWITCH:
            printf("(switch ");
            print_expr(s->switch_stmt.expr);
            indent++;
            print_newline();
            for(SwitchCase *it = s->switch_stmt.cases; it != s->switch_stmt.cases + s->switch_stmt.num_cases; it++){
                print_stmt_case(*it);
            }
            indent--;
            printf(")");
            break;

        case STMT_ASSIGN:
            printf("(%s ", tokenkind_to_str[s->assign.op]);
            print_expr(s->assign.left);
            printf(" ");
            print_expr(s->assign.right);
            printf(")");

            break;

        case STMT_INIT:
            printf("(= ");
            printf("%s ", s->init.var_name);
            print_expr(s->init.expr);
            printf(")");

            break;

        case STMT_EXPR:
            print_expr(s->expr);
            break;

        default:
            break;
    }
}