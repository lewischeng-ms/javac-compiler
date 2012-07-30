#include "javac.h"

// If set to true, 'entering ...' will be printed.
#define PARSER_TRACE 0

static tree_t parser_translation_unit();
static tree_t parser_external_decl();
static tree_t parser_prototype_decl();
static tree_t parser_record_def();
static tree_t parser_function_def();
static void   parser_function_head(tree_t decl);
static tree_t parser_variable_decl_list();
static tree_t parser_stmt_list();
static tree_t parser_type_specifier();
static tree_t parser_parameter_list();
static tree_t parser_variable_decl();
static tree_t parser_parameter_decl();
static tree_t parser_stmt();
static tree_t parser_compound_stmt();
static tree_t parser_expr_stmt();
static tree_t parser_selection_stmt();
static tree_t parser_iteration_stmt();
static tree_t parser_jump_stmt();
static tree_t parser_expr();
static tree_t parser_assignment_expr();
static tree_t parser_unary_expr(tree_t unary_subst);
static tree_t parser_logical_or_expr(tree_t unary_subst);
static tree_t parser_postfix();
static tree_t parser_primary();
static tree_t parser_logical_and_expr(tree_t unary_subst);
static tree_t parser_equality_expr(tree_t unary_subst);
static tree_t parser_relational_expr(tree_t unary_subst);
static tree_t parser_additive_expr(tree_t unary_subst);
static tree_t parser_mult_expr(tree_t unary_subst);

static bool parser_next_token_is_eof();
static void parser_eat_next_token(int token_type, const char *text);
static bool parser_next_token_is(int token_type, const char *text);
static bool parser_next_token_indicates_typespec();
static void parser_current_token_should_be(int token_type, const char *text);
static bool parser_current_token_is(int token_type, const char *text);

static tree_t parser_id();
static tree_t parser_int_const();
static tree_t parser_char_const();
static tree_t parser_string_const();
static tree_t parser_null();

static void _parser_print_fun(tree_t decl);
static void _parser_print_type(tree_t typespec);
static void _parser_print_var(tree_t var);
static void _parser_print_param(tree_t param);

static phashtab_t typetab;

void parser_init()
{
	typetab = hashtab_create(31);
}

void parser_finit()
{
	hashtab_destroy(typetab);
}

tree_t parser_file()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'file'\n");
#	endif
	
	return parser_translation_unit();
}

void parser_visit_tree(tree_node_visitor_t *visitor, tree_t tree)
{
	assert(visitor);
	assert(tree);
	
	visitor->visit_tree(visitor, tree, 0);
}

// translation_unit : external_decl
// translation_unit : translation_unit external_decl
static tree_t parser_translation_unit()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'translation unit'\n");
#	endif

	if (parser_next_token_is_eof()) {
		fatal("empty translation unit is not allowed");
		return NULL;
	}
	
	tree_t tu = TREE_ALLOC(tree_list_t);
	TREE_NODE_KIND(tu) = NODE_KIND_LIST;
	TREE_NODE_LNO(tu) = lineno;
	TREE_LIST_KIND(tu) = LIST_KIND_TU;
	
	// Multiple external decls.
	TREE_LIST(tu) = slist_create();
	slist_iter_t iter;
	slist_iter_begin(TREE_LIST(tu), &iter);
	do {
		tree_t edecl = parser_external_decl();
		slist_insert(&iter, &edecl, sizeof(ptree_t));
		
		// Move to the end of the list to preserve the insertion sequence.
		slist_iter_move_next(&iter);
	} while (!parser_next_token_is_eof());
	
	return tu;
}

// external_decl : prototype_decl
//               | function_def
//               | record_def
static tree_t parser_external_decl()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'external decl'\n");
#	endif
	
	lex_peek_token();
	if (LATYPE_IS(TOKEN_TYPE_KEYWORD)) {
		if (LATEXT_EQUALS("native"))
			return parser_prototype_decl();
		else if (LATEXT_EQUALS("record"))
			return parser_record_def();
	}
	
	// function decl.
	return parser_function_def();
}

// prototype_decl : NATIVE function_head SEMICOLON
static tree_t parser_prototype_decl()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'prototype decl'\n");
#	endif
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "native");
	
	tree_t proto = TREE_ALLOC(tree_decl_t);
	TREE_NODE_KIND(proto) = NODE_KIND_DECL;
	TREE_NODE_LNO(proto) = lineno;
	TREE_DECL_KIND(proto) = DECL_KIND_FUNCTION;
	TREE_DECL_NATIVE(proto) = true;
	
	parser_function_head(proto);
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, ";");
	
	return proto;
}

// record_def : RECORD ID LBRACE variable_decl_list RBRACE
static tree_t parser_record_def()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'record def'\n");
#	endif
	
	tree_t rdef = TREE_ALLOC(tree_decl_t);
	TREE_NODE_KIND(rdef) = NODE_KIND_DECL;
	TREE_NODE_LNO(rdef) = lineno;
	TREE_DECL_KIND(rdef) = DECL_KIND_TYPENAME;
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "record");
	
	TREE_DECL_ID(rdef) = parser_id();
	hashtab_insert(typetab, CURRTEXT(), NULL, 0);

#	ifndef NDEBUG
	assert(hashtab_lookup(typetab, CURRTEXT(), NULL, 0));
#	endif
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "{");
	
	TREE_DECL_VARS(rdef) = parser_variable_decl_list();
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "}");
	
#	ifndef NDEBUG
	printf("    record: %s\n", TREE_ID_NAME(TREE_DECL_ID(rdef)));
#	endif

	return rdef;
}

// function_def : function_head LBRACE variable_decl_list stmt_list RBRACE
//              | function_head LBRACE                    stmt_list RBRACE
static tree_t parser_function_def()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'function def'\n");
#	endif

	tree_t fun = TREE_ALLOC(tree_decl_t);
	TREE_NODE_KIND(fun) = NODE_KIND_DECL;
	TREE_NODE_LNO(fun) = lineno;
	TREE_DECL_KIND(fun) = DECL_KIND_FUNCTION;
	TREE_DECL_NATIVE(fun) = false;
	
	parser_function_head(fun);
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "{");
	
	TREE_DECL_VARS(fun) = parser_variable_decl_list();
	
	TREE_DECL_STMTS(fun) = parser_stmt_list();
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "}");
	
	return fun;
}

// function_head : type_specifier ID LPAREN parameter_list RPAREN
//               | type_specifier ID LPAREN                RPAREN
static void parser_function_head(tree_t decl)
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'function head'\n");
#	endif
	
	TREE_DECL_TYPESPEC(decl) = parser_type_specifier();
	
	TREE_DECL_ID(decl) = parser_id();
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "(");
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, ")")) {
		lex_next_token();
		TREE_DECL_PARAMS(decl) = NULL;
	} else {
		TREE_DECL_PARAMS(decl) = parser_parameter_list();
		parser_eat_next_token(TOKEN_TYPE_KEYWORD, ")");
	}
	
#	ifndef NDEBUG
	printf("    fun: ");
	_parser_print_fun(decl);
#	endif
}

// variable_decl_list : variable_decl
// variable_decl_list : variable_decl_list variable_decl
static tree_t parser_variable_decl_list()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'variable decl list'\n");
#	endif
	
	tree_t vars = TREE_ALLOC(tree_list_t);
	TREE_NODE_KIND(vars) = NODE_KIND_LIST;
	TREE_NODE_LNO(vars) = lineno;
	TREE_LIST_KIND(vars) = LIST_KIND_VARS;
	
	slist_iter_t iter;
	TREE_LIST(vars) = slist_create();
	slist_iter_begin(TREE_LIST(vars), &iter);
	
	// There must exist at least one local variable
	// though it seems very strange?!?!
	do {
		tree_t var = parser_variable_decl();
		slist_insert(&iter, &var, sizeof(ptree_t));
		slist_iter_move_next(&iter); // Insert to the end.
	} while (parser_next_token_indicates_typespec());
	
	return vars;
}

// stmt_list : stmt
// stmt_list : stmt_list stmt
static tree_t parser_stmt_list() {
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'stmt list'\n");
#	endif
	
	tree_t stmts = TREE_ALLOC(tree_list_t);
	TREE_NODE_KIND(stmts) = NODE_KIND_LIST;
	TREE_NODE_LNO(stmts) = lineno;
	TREE_LIST_KIND(stmts) = LIST_KIND_STMTS;
	
	slist_iter_t iter;
	TREE_LIST(stmts) = slist_create();
	slist_iter_begin(TREE_LIST(stmts), &iter);
	
	// There must be at least one statement.
	do {
		tree_t stmt = parser_stmt();
		slist_insert(&iter, &stmt, sizeof(ptree_t));
		slist_iter_move_next(&iter); // Insert to the end.
	} while (!parser_next_token_is(TOKEN_TYPE_KEYWORD, "}"));
	
	return stmts;
}

// type_specifier : INT
//                | STRING
//                | CHAR
//                | ID
// type_specifier : type_specifier LRBRACKET
static tree_t parser_type_specifier()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'type specifier'\n");
#	endif
	
	tree_t typespec = TREE_ALLOC(tree_typespec_t);
	TREE_NODE_KIND(typespec) = NODE_KIND_TYPESPEC;
	TREE_NODE_LNO(typespec) = lineno;

	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, NULL)) {
		lex_next_token();
		
		if (CURRTEXT_EQUALS("int"))
			TREE_TYPESPEC_KIND(typespec) = TYPESPEC_INT;
		else if (CURRTEXT_EQUALS("string"))
			TREE_TYPESPEC_KIND(typespec) = TYPESPEC_STRING;
		else if (CURRTEXT_EQUALS("char"))
			TREE_TYPESPEC_KIND(typespec) = TYPESPEC_CHAR;
		else
			fatal("unknown type specifier %s", CURRTEXT());
	} else if (parser_next_token_is(TOKEN_TYPE_IDENTIFIER, NULL)) {
		TREE_TYPESPEC_KIND(typespec) = TYPESPEC_ID;
		TREE_TYPESPEC_ID(typespec) = parser_id();
	} else {
		fatal("unknown type specifier");
	}
	
	// Is it an array?
	TREE_TYPESPEC_ARRAY(typespec) = false;
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "[")) {
		TREE_TYPESPEC_ARRAY(typespec) = true;
		TREE_TYPESPEC_DIM(typespec) = 0;
		
		do {
			// Skip '['.
			lex_next_token();
			
			// If meet '[expr]', we consume the '[' and left 'expr]'
			// to 'primary'. (the current token is '[')
			if (!parser_next_token_is(TOKEN_TYPE_KEYWORD, "]")) {
				// dim is zero:  new x[i], type is not array,
				// dim is nonzero: new x[]...[i], type is array.
				TREE_TYPESPEC_ARRAY(typespec) = (TREE_TYPESPEC_DIM(typespec) > 0);
				break;
			}
			
			// Skip ']'.
			lex_next_token();

			TREE_TYPESPEC_DIM(typespec)++;
		} while (parser_next_token_is(TOKEN_TYPE_KEYWORD, "["));
	}
	
#	ifndef NDEBUG
	printf("    type: ");
	_parser_print_type(typespec);
	printf("\n");
#	endif

	return typespec;
}

// parameter_list : parameter_decl
// parameter_list : parameter_list COMMA parameter_decl
static tree_t parser_parameter_list()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'parameter list'\n");
#	endif
	
	tree_t params = TREE_ALLOC(tree_list_t);
	TREE_NODE_KIND(params) = NODE_KIND_LIST;
	TREE_NODE_LNO(params) = lineno;
	TREE_LIST_KIND(params) = LIST_KIND_PARAMS;
	
	slist_iter_t iter;
	TREE_LIST(params) = slist_create();
	slist_iter_begin(TREE_LIST(params), &iter);
	
	// There must exist at least one parameter.
	while (true) {
		tree_t param = parser_parameter_decl();
		slist_insert(&iter, &param, sizeof(ptree_t));
		slist_iter_move_next(&iter); // Insert to the end.
		
		if (!parser_next_token_is(TOKEN_TYPE_KEYWORD, ","))
			break;
		
		// Skip ','.
		lex_next_token();
	}
	
	return params;
}

// variable_decl : type_specifier id_list SEMICOLON
static tree_t parser_variable_decl()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'variable decl'\n");
#	endif
	
	tree_t var = TREE_ALLOC(tree_decl_t);
	TREE_NODE_KIND(var) = NODE_KIND_DECL;
	TREE_NODE_LNO(var) = lineno;
	TREE_DECL_KIND(var) = DECL_KIND_VARIABLE;
	TREE_DECL_PARAM(var) = false; // Not a parameter.
	
	TREE_DECL_TYPESPEC(var) = parser_type_specifier();
	
	TREE_DECL_ID(var) = parser_id();

	// This grammer was not defined in language specification,
	// but appeared in queens.java.
	// For example: int a, b;
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, ","))
		fatal("multiple variable definitions in one statement are not allowed");
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, ";");
	
#	ifndef NDEBUG
	printf("    var: ");
	_parser_print_var(var);
	printf("\n");
#	endif
	
	return var;
}

// parameter_decl : type_specifier ID
static tree_t parser_parameter_decl()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'parameter decl'\n");
#	endif
	
	tree_t param = TREE_ALLOC(tree_decl_t);
	TREE_NODE_KIND(param) = NODE_KIND_DECL;
	TREE_NODE_LNO(param) = lineno;
	TREE_DECL_KIND(param) = DECL_KIND_VARIABLE;
	TREE_DECL_PARAM(param) = true;
	
	TREE_DECL_TYPESPEC(param) = parser_type_specifier();
	
	TREE_DECL_ID(param) = parser_id();
	
#	ifndef NDEBUG
	printf("    param: ");
	_parser_print_param(param);
	printf("\n");
#	endif
	
	return param;
}

// stmt : compound_stmt
//      | expr_stmt
//      | selection_stmt
//      | iteration_stmt
//      | jump_stmt
static tree_t parser_stmt()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'stmt'\n");
#	endif
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "{"))
		return parser_compound_stmt();
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "if"))
		return parser_selection_stmt();
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "while") ||
		parser_next_token_is(TOKEN_TYPE_KEYWORD, "for"))
		return parser_iteration_stmt();
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "return") ||
		parser_next_token_is(TOKEN_TYPE_KEYWORD, "break") ||
		parser_next_token_is(TOKEN_TYPE_KEYWORD, "continue"))
		return parser_jump_stmt();
	
	// Not one of above statements, assume it to be expression statement.
	return parser_expr_stmt();
}

// compound_stmt : LBRACE stmt_list RBRACE
//               | LBRACE           RBRACE
static tree_t parser_compound_stmt()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'compound stmt'\n");
#	endif
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "{");
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "}")) {
		lex_next_token();
		
#		ifndef NDEBUG
		printf("    compound stmt: empty\n");
#		endif
		
		return NULL;
	}
	
	tree_t compound = TREE_ALLOC(tree_stmt_t);
	TREE_NODE_KIND(compound) = NODE_KIND_STMT;
	TREE_STMT_KIND(compound) = STMT_KIND_COMPOUND;
	TREE_NODE_LNO(compound) = lineno;
	
	TREE_STMT_BODY(compound) = parser_stmt_list();
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "}");
	
#	ifndef NDEBUG
	printf("    compound stmt\n");
#	endif
	
	return compound;
}

// expr_stmt : expr SEMICOLON
static tree_t parser_expr_stmt()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'expr stmt'\n");
#	endif
	
	tree_t stmt = TREE_ALLOC(tree_stmt_t);
	TREE_NODE_KIND(stmt) = NODE_KIND_STMT;
	TREE_NODE_LNO(stmt) = lineno;
	TREE_STMT_KIND(stmt) = STMT_KIND_EXPR;
	
	TREE_STMT_EXP(stmt) = parser_expr();
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, ";");
	
#	ifndef NDEBUG
	printf("    expr stmt\n");
#	endif
	
	return stmt;
}

// selection_stmt : IF LPAREN expr RPAREN stmt
//                | IF LPAREN expr RPAREN stmt ELSE stmt
static tree_t parser_selection_stmt()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'selection stmt'\n");
#	endif
	
	tree_t stmt = TREE_ALLOC(tree_stmt_t);
	TREE_NODE_KIND(stmt) = NODE_KIND_STMT;
	TREE_NODE_LNO(stmt) = lineno;
	TREE_STMT_KIND(stmt) = STMT_KIND_IF;
	
#	ifndef NDEBUG
	printf("    sel stmt: if\n");
#	endif
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "if");
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, "(");
	
	TREE_STMT_EXP(stmt) = parser_expr();
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, ")");
	
	TREE_IF_THEN(stmt) = parser_stmt();
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "else")) {
#		ifndef NDEBUG
		printf("    sel stmt: else\n");
#		endif
		
		lex_next_token();
		
		TREE_IF_ELSE(stmt) = parser_stmt();
	} else {
		TREE_IF_ELSE(stmt) = NULL;
	}
	
	return stmt;
}

// iteration_stmt : WHILE LPAREN expr RPAREN stmt
//                | FOR LPAREN expr_stmt expr_stmt expr RPAREN stmt
//                | FOR LPAREN expr_stmt expr_stmt      RPAREN stmt
//                | FOR LPAREN expr_stmt SEMICOLON expr RPAREN stmt
//                | FOR LPAREN expr_stmt SEMICOLON      RPAREN stmt
//                | FOR LPAREN SEMICOLON expr_stmt expr RPAREN stmt
//                | FOR LPAREN SEMICOLON expr_stmt      RPAREN stmt
//                | FOR LPAREN SEMICOLON SEMICOLON expr RPAREN stmt
//                | FOR LPAREN SEMICOLON SEMICOLON      RPAREN stmt
static tree_t parser_iteration_stmt()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'iteration stmt'\n");
#	endif
	
	tree_t stmt = NULL;
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "while")) {
#		ifndef NDEBUG
		printf("    iter stmt: while\n");
#		endif
		
		lex_next_token();
		
		stmt = TREE_ALLOC(tree_stmt_t);
		TREE_NODE_KIND(stmt) = NODE_KIND_STMT;
		TREE_NODE_LNO(stmt) = lineno;
		TREE_STMT_KIND(stmt) = STMT_KIND_WHILE;
		
		parser_eat_next_token(TOKEN_TYPE_KEYWORD, "(");
		
		TREE_STMT_EXP(stmt) = parser_expr();
		
		parser_eat_next_token(TOKEN_TYPE_KEYWORD, ")");
		
		TREE_STMT_BODY(stmt) = parser_stmt();
		
	} else if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "for")) {
#		ifndef NDEBUG
		printf("    iter stmt: for\n");
#		endif
		
		lex_next_token();
		
		stmt = TREE_ALLOC(tree_stmt_t);
		TREE_NODE_KIND(stmt) = NODE_KIND_STMT;
		TREE_NODE_LNO(stmt) = lineno;
		TREE_STMT_KIND(stmt) = STMT_KIND_FOR;
		
		parser_eat_next_token(TOKEN_TYPE_KEYWORD, "(");
		
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, ";")) {
			lex_next_token();
			
			TREE_FOR_INIT(stmt) = NULL;
		} else {
			TREE_FOR_INIT(stmt) = parser_expr_stmt();
		}
			
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, ";")) {
			lex_next_token();
			
			TREE_STMT_EXP(stmt) = NULL;
		} else {
			TREE_STMT_EXP(stmt) = parser_expr_stmt();
		}
			
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, ")")) {
			TREE_FOR_INCR(stmt) = NULL;
		} else {
			TREE_FOR_INCR(stmt) = parser_expr();
		}
		
		parser_eat_next_token(TOKEN_TYPE_KEYWORD, ")");
		
		TREE_STMT_BODY(stmt) = parser_stmt();
	}
	
	return stmt;
}

// jump_stmt : RETURN expr SEMICOLON
//           | BREAK SEMICOLON
//           | CONTINUE SEMICOLON
static tree_t parser_jump_stmt()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'jump stmt'\n");
#	endif
	
	tree_t stmt = TREE_ALLOC(tree_stmt_t);
	TREE_NODE_KIND(stmt) = NODE_KIND_STMT;
	TREE_NODE_LNO(stmt) = lineno;
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "return")) {
#		ifndef NDEBUG
		printf("    jump stmt: return\n");
#		endif
		
		lex_next_token();
		
		TREE_STMT_KIND(stmt) = STMT_KIND_RETURN;
		TREE_STMT_EXP(stmt) = parser_expr();
	} else if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "break")) {
#		ifndef NDEBUG
		printf("    jump stmt: break\n");
#		endif
		
		lex_next_token();
		
		TREE_STMT_KIND(stmt) = STMT_KIND_BREAK;
	} else if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "continue")) {
#		ifndef NDEBUG
		printf("    jump stmt: continue\n");
#		endif
		
		lex_next_token();
		
		TREE_STMT_KIND(stmt) = STMT_KIND_CONTINUE;
	}
	
	parser_eat_next_token(TOKEN_TYPE_KEYWORD, ";");
	
	return stmt;
}

// expr : assignment_expr
// expr : expr COMMA assignment_expr
static tree_t parser_expr()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'expr'\n");
#	endif
	
	tree_t expr = TREE_ALLOC(tree_list_t);
	TREE_NODE_KIND(expr) = NODE_KIND_LIST;
	TREE_NODE_LNO(expr) = lineno;
	TREE_LIST_KIND(expr) = LIST_KIND_EXPR;
	
	slist_iter_t iter;
	TREE_LIST(expr) = slist_create();
	slist_iter_begin(TREE_LIST(expr), &iter);
	
	// There must be at least one assignment expr.
	while (true) {
		tree_t assgn = parser_assignment_expr();

		slist_insert(&iter, &assgn, sizeof(ptree_t));
		slist_iter_move_next(&iter); // Insert to the end.
		
		if (!parser_next_token_is(TOKEN_TYPE_KEYWORD, ","))
			break;
		
#		ifndef NDEBUG
		printf("    op: ,\n");
#		endif
		
		lex_next_token();
	}
	
	return expr;
}

// assignment_expr : logical_or_expr
// assignment_expr : unary_expr ASSIGN assignment_expr
static tree_t parser_assignment_expr()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'assignment expr'\n");
#	endif
	
	tree_t unary = parser_unary_expr(NULL);
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "=")) {
#		ifndef NDEBUG
		printf("    op: =\n");
#		endif
		
		lex_next_token();
		
		tree_t exp = TREE_ALLOC(tree_exp_t);
		TREE_NODE_KIND(exp) = NODE_KIND_EXP;
		TREE_NODE_LNO(exp) = lineno;
		TREE_EXP_OP(exp) = EXP_OP_ASSIGNMENT;
		
		TREE_EXP_FIRST(exp) = unary;
		
		TREE_EXP_SECOND(exp) = parser_assignment_expr();
		
		return exp;
	}
	
	return parser_logical_or_expr(unary);
}

// unary_expr : postfix
// unary_expr : PLUS  unary_expr
//            | MINUS unary_expr
//            | NOT   unary_expr
static tree_t parser_unary_expr(tree_t unary_subst)
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'unary expr'\n");
#	endif

	if (unary_subst)
		return unary_subst;
	
	tree_t exp = TREE_ALLOC(tree_exp_t);
	TREE_NODE_KIND(exp) = NODE_KIND_EXP;
	TREE_NODE_LNO(exp) = lineno;
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "+")) {
#		ifndef NDEBUG
		printf("    op: +u\n");
#		endif
		
		lex_next_token();
		
		TREE_EXP_OP(exp) = EXP_OP_U_PLUS;
		TREE_EXP_FIRST(exp) = parser_unary_expr(NULL);
		return exp;
	}
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "-")) {
#		ifndef NDEBUG
		printf("    op: -u\n");
#		endif
		
		lex_next_token();
		
		TREE_EXP_OP(exp) = EXP_OP_U_MINUS;
		TREE_EXP_FIRST(exp) = parser_unary_expr(NULL);
		return exp;
	}
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "!")) {
#		ifndef NDEBUG
		printf("    op: !\n");
#		endif
		
		lex_next_token();
		
		TREE_EXP_OP(exp) = EXP_OP_NOT;
		TREE_EXP_FIRST(exp) = parser_unary_expr(NULL);
		return exp;
	}
	
	return parser_postfix();
}

// logical_or_expr : logical_and_expr
// logical_or_expr : logical_or_expr OR logical_and_expr
static tree_t parser_logical_or_expr(tree_t unary_subst)
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'logical or expr'\n");
#	endif
	
	tree_t exp = parser_logical_and_expr(unary_subst);
	
	while (parser_next_token_is(TOKEN_TYPE_KEYWORD, "||")) {
#		ifndef NDEBUG
		printf("    op: ||\n");
#		endif
		
		lex_next_token();
		
		tree_t new_exp = TREE_ALLOC(tree_exp_t);
		TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
		TREE_NODE_LNO(new_exp) = lineno;
		TREE_EXP_OP(new_exp) = EXP_OP_LOGICAL_OR;
		
		TREE_EXP_FIRST(new_exp) = exp;
		exp = new_exp;
		
		TREE_EXP_SECOND(new_exp) = parser_logical_and_expr(NULL);
	}
	
	return exp;
}

// postfix : primary
// postfix : postfix LBRACKET expr RBRACKET
//         | postfix LPAREN expr RPAREN
//         | postfix LPAREN      RPAREN
//         | postfix DOT ID
static tree_t parser_postfix()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'postfix'\n");
#	endif
	
	tree_t exp = parser_primary();
	tree_t new_exp;
	
	while (true) {
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "(")) {
#			ifndef NDEBUG
			printf("    op: ()\n");
#			endif
			
			lex_next_token();
			
			new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_CALL;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
			
			if (parser_next_token_is(TOKEN_TYPE_KEYWORD, ")")) {
				lex_next_token();
				
				TREE_EXP_SECOND(new_exp) = NULL;
				
				continue;
			}
			
			TREE_EXP_SECOND(new_exp) = parser_expr();
			
			parser_eat_next_token(TOKEN_TYPE_KEYWORD, ")");
			
			continue;
		}
		
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "[")) {
#			ifndef NDEBUG
			printf("    op: []\n");
#			endif
			
			lex_next_token();
			
			new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_INDEX;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
			
			TREE_EXP_SECOND(new_exp) = parser_expr();
			
			parser_eat_next_token(TOKEN_TYPE_KEYWORD, "]");
			
			continue;
		}
		
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, ".")) {
#			ifndef NDEBUG
			printf("    op: .\n");
#			endif
			
			lex_next_token();
			
			new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_DOT;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
			
			TREE_EXP_SECOND(new_exp) = parser_id();
			
			continue;
		}
		
		break;
	}

	return exp;
}

// primary : ID
//         | NULL
//         | INTEGER
//         | CHARACTER
//         | STRING_LITERAL
//         | LPAREN expr RPAREN
//         | NEW type_specifier LBRACKET expr RBRACKET
//         | NEW ID
static tree_t parser_primary()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'primary'\n");
#	endif
	
	if (parser_next_token_is(TOKEN_TYPE_IDENTIFIER, NULL))
		return parser_id();
	
	if (parser_next_token_is(TOKEN_TYPE_INT_CONST, NULL))
		return parser_int_const();
	
	if (parser_next_token_is(TOKEN_TYPE_CHAR_CONST, NULL))
		return parser_char_const();
	
	if (parser_next_token_is(TOKEN_TYPE_STRING_CONST, NULL))
		return parser_string_const();
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "null"))
		return parser_null();
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "(")) {
		lex_next_token();
		
		tree_t exp = parser_expr();
		
		parser_eat_next_token(TOKEN_TYPE_KEYWORD, ")");
		
		return exp;
	}
	
	if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "new")) {
#		ifndef NDEBUG
		printf("    op: new\n");
#		endif
		
		lex_next_token();
		
		tree_t exp = TREE_ALLOC(tree_exp_t);
		TREE_NODE_KIND(exp) = NODE_KIND_EXP;
		TREE_NODE_LNO(exp) = lineno;
		TREE_EXP_OP(exp) = EXP_OP_NEW;
		
		TREE_EXP_FIRST(exp) = parser_type_specifier();
		
		int typespec_kind = TREE_TYPESPEC_KIND(TREE_EXP_FIRST(exp));
		switch (typespec_kind) {
		case TYPESPEC_CHAR:
		case TYPESPEC_INT:
		case TYPESPEC_STRING:
			parser_current_token_should_be(TOKEN_TYPE_KEYWORD, "[");
			TREE_EXP_SECOND(exp) = parser_expr();
			parser_eat_next_token(TOKEN_TYPE_KEYWORD, "]");
			break;
		case TYPESPEC_ID:
			if (parser_current_token_is(TOKEN_TYPE_KEYWORD, "[")) {
				TREE_EXP_SECOND(exp) = parser_expr();
				parser_eat_next_token(TOKEN_TYPE_KEYWORD, "]");
			} else {
				TREE_EXP_SECOND(exp) = NULL;
			}
			break;
		default:
			assert(false);
			break;
		}
		
		return exp;
	}

	fatal("primary expression expected");
	
	return NULL;
}

// logical_and_expr : equality_expr
// logical_and_expr : logical_and_expr AND equality_expr
static tree_t parser_logical_and_expr(tree_t unary_subst)
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'logical and expr'\n");
#	endif
	
	tree_t exp = parser_equality_expr(unary_subst);
	
	while (parser_next_token_is(TOKEN_TYPE_KEYWORD, "&&")) {
#		ifndef NDEBUG
		printf("    op: &&\n");
#		endif
		
		lex_next_token();
		
		tree_t new_exp = TREE_ALLOC(tree_exp_t);
		TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
		TREE_NODE_LNO(new_exp) = lineno;
		TREE_EXP_OP(new_exp) = EXP_OP_LOGICAL_AND;
		
		TREE_EXP_FIRST(new_exp) = exp;
		exp = new_exp;
		
		TREE_EXP_SECOND(new_exp) = parser_equality_expr(NULL);
	}
	
	return exp;
}

// equality_expr : relational_expr
// equality_expr : equality_expr EQ  relational_expr
//               | equality_expr NEQ relational_expr
static tree_t parser_equality_expr(tree_t unary_subst)
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'equality expr'\n");
#	endif
	
	tree_t exp = parser_relational_expr(unary_subst);
	
	while (true) {
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "==")) {
#			ifndef NDEBUG
			printf("    op: ==\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_EQ;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
			
			TREE_EXP_SECOND(new_exp) = parser_relational_expr(NULL);
			
			continue;
		}
		
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "!=")) {
#			ifndef NDEBUG
			printf("    op: !=\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_NEQ;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
			
			TREE_EXP_SECOND(new_exp) = parser_relational_expr(NULL);
			
			continue;
		}
		
		break;
	}
	
	return exp;
}

// relational_expr : additive_expr
// relational_expr : relational_expr LESS       additive_expr
//                 | relational_expr LESS_EQ    additive_expr
//                 | relational_expr GREATER    additive_expr
//                 | relational_expr GREATER_EQ additive_expr
static tree_t parser_relational_expr(tree_t unary_subst)
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'relational expr'\n");
#	endif
	
	tree_t exp = parser_additive_expr(unary_subst);
	
	while (true) {
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "<")) {
#			ifndef NDEBUG
			printf("    op: <\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_LESS;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
			
			TREE_EXP_SECOND(new_exp) = parser_additive_expr(NULL);
			
			continue;
		}
		
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "<=")) {
#			ifndef NDEBUG
			printf("    op: <=\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_LESS_EQ;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
			
			TREE_EXP_SECOND(new_exp) = parser_additive_expr(NULL);
			
			continue;
		}
		
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, ">")) {
#			ifndef NDEBUG
			printf("    op: >\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_GREATER;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
			
			TREE_EXP_SECOND(new_exp) = parser_additive_expr(NULL);
			
			continue;
		}
		
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, ">=")) {
#			ifndef NDEBUG
			printf("    op: >=\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_GREATER_EQ;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
			
			TREE_EXP_SECOND(new_exp) = parser_additive_expr(NULL);
			
			continue;
		}
		
		break;
	}
	
	return exp;
}

// additive_expr : mult_expr
// additive_expr : additive_expr PLUS  mult_expr
//               | additive_expr MINUS mult_expr
static tree_t parser_additive_expr(tree_t unary_subst)
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'additive expr'\n");
#	endif
	
	tree_t exp = parser_mult_expr(unary_subst);
	
	while (true) {
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "+")) {
#			ifndef NDEBUG
			printf("    op: +\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_PLUS;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
		
			TREE_EXP_SECOND(new_exp) = parser_mult_expr(NULL);
			
			continue;
		}

		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "-")) {
#			ifndef NDEBUG
			printf("    op: -\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_MINUS;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
		
			TREE_EXP_SECOND(new_exp) = parser_mult_expr(NULL);
			
			continue;
		}
		
		break;
	}
	
	return exp;
}

// mult_expr : unary_expr
// mult_expr : mult_expr MULTIPLY unary_expr
//           | mult_expr  DIVIDE  unary_expr
//           | mult_expr  MODULO  unary_expr
static tree_t parser_mult_expr(tree_t unary_subst)
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'mult expr'\n");
#	endif
	
	tree_t exp = parser_unary_expr(unary_subst);
	
	while (true) {
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "*")) {
#			ifndef NDEBUG
			printf("    op: *\n");
#			endif

			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_MULTIPLY;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
		
			TREE_EXP_SECOND(new_exp) = parser_unary_expr(NULL);
			
			continue;
		}
		
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "/")){
#			ifndef NDEBUG
			printf("    op: /\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_DIVIDE;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
		
			TREE_EXP_SECOND(new_exp) = parser_unary_expr(NULL);
			
			continue;
		}
		
		if (parser_next_token_is(TOKEN_TYPE_KEYWORD, "%")) {
#			ifndef NDEBUG
			printf("    op: %\n");
#			endif
			
			lex_next_token();
			
			tree_t new_exp = TREE_ALLOC(tree_exp_t);
			TREE_NODE_KIND(new_exp) = NODE_KIND_EXP;
			TREE_NODE_LNO(new_exp) = lineno;
			TREE_EXP_OP(new_exp) = EXP_OP_MODULO;
			
			TREE_EXP_FIRST(new_exp) = exp;
			exp = new_exp;
		
			TREE_EXP_SECOND(new_exp) = parser_unary_expr(NULL);
			
			continue;
		}
		
		break;
	}
	
	return exp;
}



static bool parser_next_token_is_eof()
{
	lex_peek_token();
	
	return LAEOF();
}

// When token_type is keyword:
//     if text is not null, then text is also compared.
// This function fetches a token.
static void parser_eat_next_token(int token_type, const char *text)
{
	lex_next_token();
	
	parser_current_token_should_be(token_type, text);
}

// When token_type is keyword:
//     if text is not null, then text is also compared.
// This function only peeks a token.
static bool parser_next_token_is(int token_type, const char *text)
{
	lex_peek_token();
	
	switch (token_type) {
	case TOKEN_TYPE_KEYWORD:
		if (LAEOF() ||
			!LATYPE_IS(TOKEN_TYPE_KEYWORD) ||
			(text ? !LATEXT_EQUALS(text) : 0))
			return false;
		break;
	default:
		if (LAEOF() ||
			!LATYPE_IS(token_type))
			return false;
		break;
	}
	
	return true;
}

// The next token should be int, char, string or typename
static bool parser_next_token_indicates_typespec()
{
	lex_peek_token();
	
	if (LAEOF())
		return false;
	
	if (LATYPE_IS(TOKEN_TYPE_KEYWORD) &&
		(LATEXT_EQUALS("int") ||
		 LATEXT_EQUALS("char") ||
		 LATEXT_EQUALS("string")))
		return true;
	
	if (LATYPE_IS(TOKEN_TYPE_IDENTIFIER) &&
		hashtab_lookup(typetab, LATEXT(), NULL, 0))
		return true;

	return false;
}

// When token_type is keyword:
//     if text is not null, then text is also compared.
static void parser_current_token_should_be(int token_type, const char *text)
{
	switch (token_type) {
	case TOKEN_TYPE_KEYWORD:
		if (CURREOF() ||
			!CURRTYPE_IS(TOKEN_TYPE_KEYWORD) ||
			(text ? !CURRTEXT_EQUALS(text) : false))
			fatal("keyword '%s' expected", text);
		break;
	case TOKEN_TYPE_IDENTIFIER:
		if (CURREOF() ||
			!CURRTYPE_IS(TOKEN_TYPE_IDENTIFIER))
			fatal("identifier expected");
		break;
	case TOKEN_TYPE_INT_CONST:
		if (CURREOF() ||
			!CURRTYPE_IS(TOKEN_TYPE_INT_CONST))
			fatal("int const expected");
		break;
	case TOKEN_TYPE_CHAR_CONST:
		if (CURREOF() ||
			!CURRTYPE_IS(TOKEN_TYPE_CHAR_CONST))
			fatal("char const expected");
		break;
	case TOKEN_TYPE_STRING_CONST:
		if (CURREOF() ||
			!CURRTYPE_IS(TOKEN_TYPE_STRING_CONST))
			fatal("string const expected");
		break;
	default:
		// How did you get here?
		assert(false);
		break;
	}
}

// When token_type is keyword:
//     if text is not null, then text is also compared.
static bool parser_current_token_is(int token_type, const char *text)
{
	switch (token_type) {
	case TOKEN_TYPE_KEYWORD:
		if (CURREOF() ||
			!CURRTYPE_IS(TOKEN_TYPE_KEYWORD) ||
			(text ? !LATEXT_EQUALS(text) : 0))
			return false;
		break;
	default:
		if (CURREOF() ||
			!CURRTYPE_IS(token_type))
			return false;
		break;
	}
	
	return true;
}



static tree_t parser_id()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'id'\n");
#	endif
	
	lex_next_token();
	parser_current_token_should_be(TOKEN_TYPE_IDENTIFIER, NULL);
	
	tree_t id = TREE_ALLOC(tree_id_t);
	TREE_NODE_KIND(id) = NODE_KIND_ID;
	TREE_NODE_LNO(id) = lineno;
	TREE_ID_NAME(id) = xstrdup(CURRTEXT());
	
#	ifndef NDEBUG
	printf("    id: %s\n", CURRTEXT());
#	endif
	
	return id;
}

static tree_t parser_int_const()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'int const'\n");
#	endif
	
	lex_next_token();
	parser_current_token_should_be(TOKEN_TYPE_INT_CONST, NULL);
	
	tree_t ic = TREE_ALLOC(tree_const_t);
	TREE_NODE_KIND(ic) = NODE_KIND_CONST;
	TREE_NODE_LNO(ic) = lineno;
	TREE_CONST_KIND(ic) = CONST_KIND_INTEGER;
	TREE_CONST_INT(ic) = CURRINT();
	
#	ifndef NDEBUG
	printf("    int const: %d\n", CURRINT());
#	endif
	
	return ic;
}

static tree_t parser_char_const()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'char const'\n");
#	endif
	
	lex_next_token();
	parser_current_token_should_be(TOKEN_TYPE_CHAR_CONST, NULL);
	
	tree_t cc = TREE_ALLOC(tree_const_t);
	TREE_NODE_KIND(cc) = NODE_KIND_CONST;
	TREE_NODE_LNO(cc) = lineno;
	TREE_CONST_KIND(cc) = CONST_KIND_CHARACTER;
	TREE_CONST_CHAR(cc) = CURRCHAR();
	
#	ifndef NDEBUG
	printf("    char const: 0x%02x\n", CURRCHAR());
#	endif
	
	return cc;
}

static tree_t parser_string_const()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'string const'\n");
#	endif
	
	lex_next_token();
	parser_current_token_should_be(TOKEN_TYPE_STRING_CONST, NULL);
	
	tree_t sc = TREE_ALLOC(tree_const_t);
	TREE_NODE_KIND(sc) = NODE_KIND_CONST;
	TREE_NODE_LNO(sc) = lineno;
	TREE_CONST_KIND(sc) = CONST_KIND_STRING;
	TREE_CONST_STRING(sc) = xstrdup(CURRTEXT());
	
#	ifndef NDEBUG
	printf("    string const: %s\n", CURRTEXT());
#	endif
	
	return sc;
}

static tree_t parser_null()
{
#	if !defined(NDEBUG) && PARSER_TRACE
	printf("entering 'null'\n");
#	endif
	
	lex_next_token();
	parser_current_token_should_be(TOKEN_TYPE_KEYWORD, "null");
	
	tree_t null = TREE_ALLOC(tree_const_t);
	TREE_NODE_KIND(null) = NODE_KIND_CONST;
	TREE_NODE_LNO(null) = lineno;
	TREE_CONST_KIND(null) = CONST_KIND_NULL;
	
#	ifndef NDEBUG
	printf("    null\n");
#	endif
	
	return null;
}



static void _parser_print_fun(tree_t decl)
{
	printf("'%s' returns ", TREE_ID_NAME(TREE_DECL_ID(decl)));
	_parser_print_type(TREE_DECL_TYPESPEC(decl));
	
	if (TREE_DECL_PARAMS(decl)) {
		printf(", with:\n");
		
		slist_iter_t iter;
		slist_iter_begin(TREE_LIST(TREE_DECL_PARAMS(decl)), &iter);
		while (slist_iter_not_end(&iter)) {
			printf("        param: ");
			_parser_print_param(SLIST_ITER_GET_T(iter, tree_t));
			printf("\n");
			
			slist_iter_move_next(&iter);
		}
	} else {
		printf(", with no params\n");
	}
}

static void _parser_print_type(tree_t typespec)
{
	if (TREE_TYPESPEC_ARRAY(typespec))
		printf("%d-dim array of ", TREE_TYPESPEC_DIM(typespec));
	
	switch (TREE_TYPESPEC_KIND(typespec)) {
	case TYPESPEC_CHAR:
		printf("char");
		break;
	case TYPESPEC_ID:
		printf(TREE_ID_NAME(TREE_TYPESPEC_ID(typespec)));
		break;
	case TYPESPEC_INT:
		printf("int");
		break;
	case TYPESPEC_STRING:
		printf("string");
		break;
	}
}

static void _parser_print_var(tree_t var)
{
	printf("'%s' of type ", TREE_ID_NAME(TREE_DECL_ID(var)));
	_parser_print_type(TREE_DECL_TYPESPEC(var));
}

static void _parser_print_param(tree_t param)
{
	printf("'%s' of type ", TREE_ID_NAME(TREE_DECL_ID(param)));
	_parser_print_type(TREE_DECL_TYPESPEC(param));
}
