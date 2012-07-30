#include "javac.h"

static void visit_tree(tree_node_visitor_t *visitor, tree_t tree, int depth);
static void visit_const(tree_node_visitor_t *visitor, tree_t tree, int depth);
static void visit_decl(tree_node_visitor_t *visitor, tree_t tree, int depth);
static void visit_exp(tree_node_visitor_t *visitor, tree_t tree, int depth);
static void visit_id(tree_node_visitor_t *visitor, tree_t tree, int depth);
static void visit_list(tree_node_visitor_t *visitor, tree_t tree, int depth);
static void visit_stmt(tree_node_visitor_t *visitor, tree_t tree, int depth);
static void visit_typespec(tree_node_visitor_t *visitor, tree_t tree, int depth);

tree_node_visitor_t print_visitor = {
	visit_tree,
	visit_const,
	visit_decl,
	visit_exp,
	visit_id,
	visit_list,
	visit_stmt,
	visit_typespec
};

static void print_space(int depth)
{
	assert(depth >= 0);
	
	int i;
	for (i = 0; i < (depth << 2); ++i)
		printf(" ");
}

static void visit_tree(tree_node_visitor_t *visitor, tree_t tree, int depth)
{
	printf("printing grammer tree:\n");
	
	assert(TREE_NODE_KIND(tree) == NODE_KIND_LIST);
	assert(TREE_LIST_KIND(tree) == LIST_KIND_TU);
	visitor->visit_list(visitor, tree, depth + 1);
}

static void visit_const(tree_node_visitor_t *visitor, tree_t tree, int depth)
{
	print_space(depth);
	
	switch (TREE_CONST_KIND(tree)) {
	case CONST_KIND_CHARACTER:
		printf("character = 0x%02x\n", TREE_CONST_CHAR(tree));
		break;
	case CONST_KIND_INTEGER:
		printf("integer = %d\n", TREE_CONST_INT(tree));
		break;
	case CONST_KIND_NULL:
		printf("null\n");
		break;
	case CONST_KIND_STRING:
		printf("string literal = \"%s\"\n", TREE_CONST_STRING(tree));
		break;
	default:
		assert(false);
		break;
	}
}

static void visit_decl(tree_node_visitor_t *visitor, tree_t tree, int depth)
{
	tree_t typespec;
	tree_t id;
	tree_t params;
	tree_t vars;
	tree_t stmts;
	
	print_space(depth);
	switch (TREE_DECL_KIND(tree)) {
	case DECL_KIND_FUNCTION:
		if (TREE_DECL_NATIVE(tree)) {
			printf("prototype decl\n");
			
			// type specifier.
			typespec = TREE_DECL_TYPESPEC(tree);
			assert(typespec);
			assert(TREE_NODE_KIND(typespec) == NODE_KIND_TYPESPEC);
			visitor->visit_typespec(visitor, typespec, depth + 1);
			
			// id.
			id = TREE_DECL_ID(tree);
			assert(id);
			assert(TREE_NODE_KIND(id) == NODE_KIND_ID);
			visitor->visit_id(visitor, id, depth + 1);
			
			// parameter list.
			params = TREE_DECL_PARAMS(tree);
			if (params) {
				assert(TREE_NODE_KIND(params) == NODE_KIND_LIST);
				assert(TREE_LIST_KIND(params) == LIST_KIND_PARAMS);
				visitor->visit_list(visitor, params, depth + 1);
			}
		} else {
			printf("function def\n");
			
			// type specifier.
			typespec = TREE_DECL_TYPESPEC(tree);
			assert(typespec);
			assert(TREE_NODE_KIND(typespec) == NODE_KIND_TYPESPEC);
			visitor->visit_typespec(visitor, typespec, depth + 1);
			
			// id.
			id = TREE_DECL_ID(tree);
			assert(id);
			assert(TREE_NODE_KIND(id) == NODE_KIND_ID);
			visitor->visit_id(visitor, id, depth + 1);
			
			// parameter list.
			params = TREE_DECL_PARAMS(tree);
			if (params) {
				assert(TREE_NODE_KIND(params) == NODE_KIND_LIST);
				assert(TREE_LIST_KIND(params) == LIST_KIND_PARAMS);
				visitor->visit_list(visitor, params, depth + 1);
			}
			
			// var decl list.
			vars = TREE_DECL_VARS(tree);
			assert(vars);
			assert(TREE_NODE_KIND(vars) == NODE_KIND_LIST);
			assert(TREE_LIST_KIND(vars) == LIST_KIND_VARS);
			visitor->visit_list(visitor, vars, depth + 1);
			
			// stmt list.
			stmts = TREE_DECL_STMTS(tree);
			assert(stmts);
			assert(TREE_NODE_KIND(stmts) == NODE_KIND_LIST);
			assert(TREE_LIST_KIND(stmts) == LIST_KIND_STMTS);
			visitor->visit_list(visitor, stmts, depth + 1);
		}
		break;
	case DECL_KIND_VARIABLE:
		if (TREE_DECL_PARAM(tree)) {
			printf("parameter decl\n");
			
			// type specifier.
			typespec = TREE_DECL_TYPESPEC(tree);
			assert(TREE_NODE_KIND(typespec) == NODE_KIND_TYPESPEC);
			visitor->visit_typespec(visitor, typespec, depth + 1);
			
			// id.
			id = TREE_DECL_ID(tree);
			assert(TREE_NODE_KIND(id) == NODE_KIND_ID);
			visitor->visit_id(visitor, id, depth + 1);
		} else {
			printf("variable decl\n");
			
			// type specifier.
			typespec = TREE_DECL_TYPESPEC(tree);
			assert(TREE_NODE_KIND(typespec) == NODE_KIND_TYPESPEC);
			visitor->visit_typespec(visitor, typespec, depth + 1);
			
			// id.
			id = TREE_DECL_ID(tree);
			assert(TREE_NODE_KIND(id) == NODE_KIND_ID);
			visitor->visit_id(visitor, id, depth + 1);
		}

		break;
	case DECL_KIND_TYPENAME:
		printf("record def\n");
		
		// id.
		id = TREE_DECL_ID(tree);
		assert(TREE_NODE_KIND(id) == NODE_KIND_ID);
		visitor->visit_id(visitor, id, depth + 1);
		
		// var decl list.
		vars = TREE_DECL_VARS(tree);
		assert(vars);
		assert(TREE_NODE_KIND(vars) == NODE_KIND_LIST);
		assert(TREE_LIST_KIND(vars) == LIST_KIND_VARS);
		visitor->visit_list(visitor, vars, depth + 1);
		break;
	default:
		assert(false);
		break;
	}
}

static void visit_exp(tree_node_visitor_t *visitor, tree_t tree, int depth)
{
	// id is exp.
	if (TREE_NODE_KIND(tree) == NODE_KIND_ID) {
		visitor->visit_id(visitor, tree, depth);
		return;
	}
	
	// const is exp.
	if (TREE_NODE_KIND(tree) == NODE_KIND_CONST) {
		visitor->visit_const(visitor, tree, depth);
		return;
	}
	
	// expr, expr, ...
	if (TREE_NODE_KIND(tree) == NODE_KIND_LIST) {
		slist_iter_t iter;
		slist_iter_begin(TREE_LIST(tree), &iter);
		while (slist_iter_not_end(&iter)) {
			tree_t exp = SLIST_ITER_GET_T(iter, tree_t);
			visitor->visit_exp(visitor, exp, depth);
			
			slist_iter_move_next(&iter);
		}
		return;
	}
	
	// the following exps are all with one specific operator.
	assert(TREE_NODE_KIND(tree) == NODE_KIND_EXP);
	
	print_space(depth);
		
	tree_t first;
	tree_t second;
	switch (TREE_EXP_OP(tree)) {
	case EXP_OP_CALL:
		printf("function call\n");
		
		// function id.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// arguments.
		second = TREE_EXP_SECOND(tree);
		if (second)
			visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_INDEX:
		printf("postfix (index)\n");
		
		// indexee.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// indexer.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_DOT:
		printf("postfix (dot)\n");
		
		// obj.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// property.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		assert(TREE_NODE_KIND(second) == NODE_KIND_ID);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_ASSIGNMENT:
		printf("assignment expr\n");
		
		// left-value.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// right-value.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_U_PLUS:
		printf("unary expr (plus)\n");
		
		// operand.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		break;
	case EXP_OP_U_MINUS:
		printf("unary expr (minus)\n");
		
		// operand.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		break;
	case EXP_OP_NOT:
		printf("unary expr (not)\n");
		
		// operand.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		break;
	case EXP_OP_LOGICAL_OR:
		printf("logical or expr\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_LOGICAL_AND:
		printf("logical and expr\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_EQ:
		printf("relational expr (eq)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_NEQ:
		printf("relational expr (neq)\n");
		
		// lhs. 
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_LESS:
		printf("relational expr (less)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_LESS_EQ:
		printf("relational expr (less eq)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_GREATER:
		printf("relational expr (greater)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_GREATER_EQ:
		printf("relational expr (greater eq)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_PLUS:
		printf("additive expr (plus)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_MINUS:
		printf("additive expr (minus)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_MULTIPLY:
		printf("mult expr (mult)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_DIVIDE:
		printf("mult expr (divide)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_MODULO:
		printf("mult expr (modulo)\n");
		
		// lhs.
		first = TREE_EXP_FIRST(tree);
		assert(first);
		
		visitor->visit_exp(visitor, first, depth + 1);
		
		// rhs.
		second = TREE_EXP_SECOND(tree);
		assert(second);
		
		visitor->visit_exp(visitor, second, depth + 1);
		break;
	case EXP_OP_NEW:
		printf("primary (new)\n");
		
		tree_t typespec = TREE_EXP_FIRST(tree);
		assert(typespec);
		assert(TREE_NODE_KIND(typespec) == NODE_KIND_TYPESPEC);
		
		// type.
		visitor->visit_typespec(visitor, typespec, depth + 1);
		
		// dim.
		tree_t dim = TREE_EXP_SECOND(tree);
		if (dim)
			visitor->visit_exp(visitor, dim, depth + 1);
		break;
	default:
		assert(false);
		break;
	}
}

static void visit_id(tree_node_visitor_t *visitor, tree_t tree, int depth)
{
	print_space(depth);
	printf("identifier = %s\n", TREE_ID_NAME(tree));
}

static void visit_list(tree_node_visitor_t *visitor, tree_t tree, int depth)
{
	slist_iter_t iter;
	
	print_space(depth);
	switch (TREE_LIST_KIND(tree)) {
	case LIST_KIND_TU:
		printf("translation unit\n");
		
		slist_iter_begin(TREE_LIST(tree), &iter);
		while (slist_iter_not_end(&iter)) {
			tree_t edecl = SLIST_ITER_GET_T(iter, tree_t);
			assert(TREE_NODE_KIND(edecl) == NODE_KIND_DECL);
			assert(TREE_DECL_KIND(edecl) == DECL_KIND_FUNCTION ||
				TREE_DECL_KIND(edecl) == DECL_KIND_TYPENAME);
			
			visitor->visit_decl(visitor, edecl, depth + 1);
			slist_iter_move_next(&iter);
		}
		break;
	case LIST_KIND_PARAMS:
		printf("parameter list\n");
		
		slist_iter_begin(TREE_LIST(tree), &iter);
		while (slist_iter_not_end(&iter)) {
			tree_t param = SLIST_ITER_GET_T(iter, tree_t);
			assert(TREE_NODE_KIND(param) == NODE_KIND_DECL);
			assert(TREE_DECL_KIND(param) == DECL_KIND_VARIABLE);
			assert(TREE_DECL_PARAM(param));
			
			visitor->visit_decl(visitor, param, depth + 1);
			slist_iter_move_next(&iter);
		}
		break;
	case LIST_KIND_VARS:
		printf("variable decl list\n");
		
		slist_iter_begin(TREE_LIST(tree), &iter);
		while (slist_iter_not_end(&iter)) {
			tree_t var = SLIST_ITER_GET_T(iter, tree_t);
			assert(TREE_NODE_KIND(var) == NODE_KIND_DECL);
			assert(TREE_DECL_KIND(var) == DECL_KIND_VARIABLE);
			assert(!TREE_DECL_PARAM(var));
			
			visitor->visit_decl(visitor, var, depth + 1);
			slist_iter_move_next(&iter);
		}
		break;
	case LIST_KIND_STMTS:
		printf("stmt list\n");
	  
		slist_iter_begin(TREE_LIST(tree), &iter);
		while (slist_iter_not_end(&iter)) {
			tree_t stmt = SLIST_ITER_GET_T(iter, tree_t);
			assert(TREE_NODE_KIND(stmt) == NODE_KIND_STMT);
			
			visitor->visit_stmt(visitor, stmt, depth + 1);
			slist_iter_move_next(&iter);
		}
		break;
	default:
		assert(false);
		break;
	}
}

static void visit_stmt(tree_node_visitor_t *visitor, tree_t tree, int depth)
{
	print_space(depth);
  
	tree_t exp;
	tree_t body;
	switch (TREE_STMT_KIND(tree)) {
	case STMT_KIND_EXPR:
		printf("expr stmt\n");
		
		// expr.
		exp = TREE_STMT_EXP(tree);
		assert(exp);
		assert(TREE_NODE_KIND(exp) == NODE_KIND_LIST);
		assert(TREE_LIST_KIND(exp) == LIST_KIND_EXPR);
			
		visitor->visit_exp(visitor, exp, depth + 1);
		break;
	case STMT_KIND_COMPOUND:
		printf("compound stmt\n");
		
		// stmt list.
		tree_t stmts = TREE_STMT_BODY(tree);
		assert(TREE_NODE_KIND(stmts) == NODE_KIND_LIST);
		assert(TREE_LIST_KIND(stmts) == LIST_KIND_STMTS);
		
		visitor->visit_list(visitor, stmts, depth + 1);
		break;
	case STMT_KIND_RETURN:
		printf("return stmt\n");
		
		// expr.
		exp = TREE_STMT_EXP(tree);
		assert(exp);
		assert(TREE_NODE_KIND(exp) == NODE_KIND_LIST);
		assert(TREE_LIST_KIND(exp) == LIST_KIND_EXPR);
			
		visitor->visit_exp(visitor, exp, depth + 1);
		break;
	case STMT_KIND_BREAK:
		printf("break stmt\n");
		break;
	case STMT_KIND_CONTINUE:
		printf("continue stmt\n");
		break;
	case STMT_KIND_IF:
		printf("if stmt\n");
		
		// expr.
		exp = TREE_STMT_EXP(tree);
		assert(exp);
		assert(TREE_NODE_KIND(exp) == NODE_KIND_LIST);
		assert(TREE_LIST_KIND(exp) == LIST_KIND_EXPR);
			
		visitor->visit_exp(visitor, exp, depth + 1);
		
		// stmt (then).
		tree_t then_part = TREE_IF_THEN(tree);
		assert(then_part);
		assert(TREE_NODE_KIND(then_part) == NODE_KIND_STMT);
		
		visitor->visit_stmt(visitor, then_part, depth + 1);
		
		// stmt (else).
		tree_t else_part = TREE_IF_ELSE(tree);
		if (else_part) {
			assert(TREE_NODE_KIND(else_part) == NODE_KIND_STMT);
		
			visitor->visit_stmt(visitor, else_part, depth + 1);
		}
		break;
	case STMT_KIND_FOR:
		printf("for stmt\n");
		
		// init.
		tree_t init = TREE_FOR_INIT(tree);
		if (init) {
			assert(TREE_NODE_KIND(init) == NODE_KIND_STMT);
			assert(TREE_STMT_KIND(init) == STMT_KIND_EXPR);

			visitor->visit_stmt(visitor, init, depth + 1);
		}
		
		// expr.
		exp = TREE_STMT_EXP(tree);
		if (exp) {
			assert(TREE_NODE_KIND(exp) == NODE_KIND_STMT);
			assert(TREE_STMT_KIND(exp) == STMT_KIND_EXPR);

			visitor->visit_stmt(visitor, exp, depth + 1);
		}
		
		// incr.
		tree_t incr = TREE_FOR_INCR(tree);
		if (incr) {
			assert(TREE_NODE_KIND(incr) == NODE_KIND_LIST);
			assert(TREE_LIST_KIND(incr) == LIST_KIND_EXPR);
			
			visitor->visit_exp(visitor, incr, depth + 1);
		}
		
		// body.
		body = TREE_STMT_BODY(tree);
		assert(body);
		assert(TREE_NODE_KIND(body) == NODE_KIND_STMT);
		
		visitor->visit_stmt(visitor, body, depth + 1);
		break;
	case STMT_KIND_WHILE:
		printf("while stmt\n");
		
		// expr.
		exp = TREE_STMT_EXP(tree);
		assert(exp);
		assert(TREE_NODE_KIND(exp) == NODE_KIND_LIST);
		assert(TREE_LIST_KIND(exp) == LIST_KIND_EXPR);
			
		visitor->visit_exp(visitor, exp, depth + 1);
		
		// body.
		body = TREE_STMT_BODY(tree);
		assert(body);
		assert(TREE_NODE_KIND(body) == NODE_KIND_STMT);
		
		visitor->visit_stmt(visitor, body, depth + 1);
		break;
	default:
		assert(false);
		break;
	}
}

static void visit_typespec(tree_node_visitor_t *visitor, tree_t tree, int depth)
{
	print_space(depth);
	printf("type specifier = ");
	
	if (TREE_TYPESPEC_ARRAY(tree))
		printf("%d-dim array of ", TREE_TYPESPEC_DIM(tree));
	
	switch (TREE_TYPESPEC_KIND(tree)) {
	case TYPESPEC_CHAR:
		printf("char");
		break;
	case TYPESPEC_ID:
		printf(TREE_ID_NAME(TREE_TYPESPEC_ID(tree)));
		break;
	case TYPESPEC_INT:
		printf("int");
		break;
	case TYPESPEC_STRING:
		printf("string");
		break;
	default:
		assert(false);
		break;
	}
	
	printf("\n");
}
