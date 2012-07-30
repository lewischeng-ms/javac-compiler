#ifndef JAVAC_H_INCLUDED
#define JAVAC_H_INCLUDED

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define CURRTEXT() (currtok.token_value.text)
#define CURRINT() (currtok.token_value.integer)
#define CURRCHAR() (currtok.token_value.character)
#define CURRTEXT_EQUALS(S) (!strcmp(CURRTEXT(), (S)))
#define CURREOF() (currtok.token_flag_eof)
#define CURRTYPE() (currtok.token_type)
#define CURRTYPE_IS(T) (CURRTYPE() == (T))

#define LATEXT() (latok.token_value.text)
#define LATEXT_EQUALS(S) (!strcmp(LATEXT(), (S)))
#define LAEOF() (latok.token_flag_eof)
#define LATYPE() (latok.token_type)
#define LATYPE_IS(T) (LATYPE() == (T))

enum TOKEN_TYPES {
	TOKEN_TYPE_KEYWORD, // Including operatos
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_INT_CONST,
	TOKEN_TYPE_CHAR_CONST,
	TOKEN_TYPE_STRING_CONST,
};

typedef struct _token_t {
	int token_type;
	union _token_value_t {
		char *text;
		int integer;
		char character;
	} token_value;
	unsigned char token_flag_valid : 1;
	unsigned char token_flag_eof : 1;
} token_t, *ptoken_t;

typedef struct _lex_state_t {
	token_t currtok;
	token_t latok;
	int lineno;
	long pos;
} lex_state_t, *plex_state_t;

void lex_init(const char *filename);
void lex_finit();
void lex_next_token();
void lex_peek_token();
void lex_print_all_tokens();

extern token_t currtok;
extern token_t latok; // look-ahead token

extern int lineno;



void *xmalloc(size_t size);
void xfree(void *p);
char *xstrdup(const char *src);
void xstat();



typedef struct _slist_node_t {
	struct _slist_node_t *plink;
	char data[0];
} slist_node_t, *pslist_node_t, *slist_t;

typedef struct _slist_iter_t {
	pslist_node_t plast;
	pslist_node_t pcurr;
} slist_iter_t, *pslist_iter_t;

void slist_iter_begin(slist_t list, pslist_iter_t piter);
void slist_iter_move_next(pslist_iter_t piter);
bool slist_iter_not_end(pslist_iter_t piter);
void *slist_iter_deref(pslist_iter_t piter);

slist_t slist_create();
void slist_destroy(slist_t list);
void slist_insert(pslist_iter_t piter, const void *pdata, size_t size);
void slist_push_front(slist_t list, const void *pdata, size_t size);
void slist_remove(pslist_iter_t piter);
bool slist_is_empty(slist_t list);
void slist_unittest();

#define SLIST_ITER_GET_T(ITER, T) (*(T *)slist_iter_deref(&(ITER)))
#define SLIST_ITER_GET_INT(ITER) SLIST_ITER_GET_T((ITER), int)



#define HASHTAB_STAT(PHT) hashtab_stat(PHT, #PHT)
#define HASHTAB_STAT_VERBOSE 0

typedef struct _hashtab_t {
	int n_buckets;
	int n_elems;
	slist_t *buckets;
} hashtab_t, *phashtab_t;

typedef struct _pair_t {
	const char *key; // Key is not allowed to change in hash table.
	char data[0];
} pair_t, *ppair_t;

phashtab_t hashtab_create(int n_buckets);
void hashtab_destroy(phashtab_t phashtab);
void hashtab_insert(phashtab_t phashtab, const char *key, const void *pdata, size_t size);
bool hashtab_lookup(phashtab_t phashtab, const char *key, void *pdata, size_t size);
void hashtab_unittest();
void hashtab_stat(phashtab_t phashtab, const char *name);



#define TREE_NODE_LNO(T) ((T)->common.lineno)
#define TREE_NODE_KIND(T) ((T)->common.node_kind)

#define TREE_LIST(T) ((T)->list.list)
#define TREE_LIST_KIND(T) ((T)->list.list_kind)

#define TREE_ALLOC(K) xmalloc(sizeof(K))

#define TREE_DECL_KIND(T) ((T)->decl_common.decl_kind)
#define TREE_DECL_NATIVE(T) ((T)->decl.flag_native)
#define TREE_DECL_PARAM(T) ((T)->decl.flag_param)
#define TREE_DECL_VARS(T) ((T)->decl.vars)
#define TREE_DECL_STMTS(T) ((T)->decl.stmts)
#define TREE_DECL_PARAMS(T) ((T)->decl.params)
#define TREE_DECL_TYPESPEC(T) ((T)->decl.typespec)
#define TREE_DECL_ID(T) ((T)->decl_common.id)

#define TREE_ID_NAME(T) ((T)->id.name)

#define TREE_TYPESPEC_KIND(T) ((T)->typespec.typespec_kind)
#define TREE_TYPESPEC_ID(T) ((T)->typespec.id)
#define TREE_TYPESPEC_ARRAY(T) ((T)->typespec.flag_array)
#define TREE_TYPESPEC_DIM(T) ((T)->typespec.array_dim)

#define TREE_STMT_KIND(T) ((T)->stmt.stmt_kind)
#define TREE_STMT_EXP(T) ((T)->stmt.exp)
#define TREE_STMT_BODY(T) ((T)->stmt.body)

#define TREE_EXP_OP(T) ((T)->exp.exp_op)
#define TREE_EXP_FIRST(T) ((T)->exp.first)
#define TREE_EXP_SECOND(T) ((T)->exp.second)

#define TREE_CONST_INT(T) ((T)->_const.value.integer)
#define TREE_CONST_CHAR(T) ((T)->_const.value.character)
#define TREE_CONST_STRING(T) ((T)->_const.value.string)
#define TREE_CONST_KIND(T) ((T)->_const.const_kind)

#define TREE_IF_THEN(T) ((T)->stmt.first)
#define TREE_IF_ELSE(T) ((T)->stmt.second)

#define TREE_FOR_INIT(T) ((T)->stmt.first)
#define TREE_FOR_INCR(T) ((T)->stmt.second)

enum TREE_NODE_KINDS {
	NODE_KIND_DECL,		// Declaration (variable, parameter, function def, prototype decl, typename)
	NODE_KIND_ID,			// Identifier
	NODE_KIND_TYPESPEC,	// Type Specifier
	NODE_KIND_EXP,			// Expression
	NODE_KIND_STMT,		// Statement
	NODE_KIND_LIST,		// List (vars, params, stmts, tu)
	NODE_KIND_CONST,		// Consts (int, char, string)
};

typedef struct _tree_common_t {
	int node_kind;
	int lineno;
} tree_common_t;

typedef union _tree_node_t *tree_t;

enum EXP_OPS {
	EXP_OP_CALL,			// '('
	EXP_OP_INDEX,			// '['
	EXP_OP_DOT,			// '.'
	EXP_OP_ASSIGNMENT,	// '='
	EXP_OP_U_PLUS,			// Unary '+'
	EXP_OP_U_MINUS,		// Unary '-'
	EXP_OP_NOT,			// '!'
	EXP_OP_LOGICAL_OR,	// '||'
	EXP_OP_LOGICAL_AND,	// '&&'
	EXP_OP_EQ,				// '=='
	EXP_OP_NEQ,			// '!='
	EXP_OP_LESS,			// '<'
	EXP_OP_LESS_EQ,		// '<='
	EXP_OP_GREATER,		// '>'
	EXP_OP_GREATER_EQ,	// '>='
	EXP_OP_PLUS,			// '+'
	EXP_OP_MINUS,			// '-'
	EXP_OP_MULTIPLY,		// '*'
	EXP_OP_DIVIDE,			// '/'
	EXP_OP_MODULO,			// '%'
	EXP_OP_NEW,			// 'new'
};

typedef struct _tree_exp_t {
	tree_common_t common;
	
	// Available for:
	//     unary_op first
	//     first binary_op second
	tree_t first;
	tree_t second;
	
	unsigned exp_op : 5;
} tree_exp_t;

enum STMT_KINDS {
	STMT_KIND_EXPR,		// Expr stmt
	STMT_KIND_COMPOUND,	// Compount stmt
	STMT_KIND_RETURN,		// Return expr_stmt
	STMT_KIND_BREAK,		// Break
	STMT_KIND_CONTINUE,	// Continue
	STMT_KIND_IF,			// If
	STMT_KIND_FOR,			// For
	STMT_KIND_WHILE,		// While
};

typedef struct _tree_stmt_t {
	tree_common_t common;
	
	// expr,
	// Available for:
	//     expr_stmt (as expr),
	//     selection_stmt (as if (expr)),
	//     iteration_stmt (as while (expr) or for (;expr;)),
	//     jump_stmt (as return expr).
	tree_t exp;
	
	// Available for:
	//     if (exp) first else second,
	//     for (first; exp; second) body.
	tree_t first;
	tree_t second;
	
	// Available for:
	//     while (exp) body,
	//     for (init; exp; incr) body.
	tree_t body;
	
	unsigned stmt_kind : 4;
} tree_stmt_t;

enum TYPESPEC_KINDS {
	TYPESPEC_INT,
	TYPESPEC_STRING,
	TYPESPEC_CHAR,
	TYPESPEC_ID
};

typedef struct _tree_typespec_t {
	tree_common_t common;
	
	// Available for:
	//     typename.
	tree_t id;
	
	unsigned typespec_kind : 2;
	unsigned flag_array : 1;
	unsigned array_dim : 8;
} tree_typespec_t;

typedef struct _tree_id_t {
	tree_common_t common;
	const char *name;
} tree_id_t;

enum CONST_KINDS {
	CONST_KIND_INTEGER,
	CONST_KIND_CHARACTER,
	CONST_KIND_STRING,
	CONST_KIND_NULL,
};

typedef struct _tree_const_t {
	tree_common_t common;
	union _value_t {
		int integer;
		char character;
		const char *string;
	} value;
	unsigned const_kind : 2;
} tree_const_t;

enum LIST_KINDS {
	LIST_KIND_TU,		// Translation unit
	LIST_KIND_VARS,	// Variable decl list
	LIST_KIND_PARAMS,	// Paramter list
	LIST_KIND_STMTS,	// Stmt list
	LIST_KIND_EXPR,	// Expr
};

typedef struct _tree_list_t {
	tree_common_t common;
	slist_t list;
	unsigned list_kind : 3;
} tree_list_t;

enum DECL_KINDS {
	DECL_KIND_VARIABLE,
	DECL_KIND_TYPENAME,
	DECL_KIND_FUNCTION,
};

typedef struct _tree_decl_common_t {
	tree_common_t common;
	tree_t id;
	unsigned decl_kind : 2;
} tree_decl_common_t;

typedef struct _tree_decl_t {
	tree_decl_common_t decl_common;
	
	// type specifier,
	// available for:
	//     prototype decl (as return type),
	//     function def (as return type),
	//     local variable,
	//     parameter.
	tree_t typespec;
	
	// parameter list,
	// available for:
	//     prototype decl,
	//     function def.
	tree_t params;
	
	// variable decl list,
	// available for:
	//     function def,
	//     record.
	tree_t vars;
	
	// stmt list,
	// available for:
	//     function def.
	tree_t stmts;
	
	unsigned flag_param : 1;
	unsigned flag_native : 1;
} tree_decl_t;

typedef union _tree_node_t {
	tree_common_t common;
	tree_exp_t exp;
	tree_stmt_t stmt;
	tree_list_t list;
	tree_decl_common_t decl_common;
	tree_decl_t decl;
	tree_id_t id;
	tree_typespec_t typespec;
	tree_const_t _const;
} tree_node_t, *tree_t;
typedef tree_t *ptree_t;

typedef struct _tree_node_visitor_t tree_node_visitor_t;

typedef void (*tree_node_visit_t)(tree_node_visitor_t *, tree_t, int);

typedef struct _tree_node_visitor_t {
	tree_node_visit_t visit_tree;
	tree_node_visit_t visit_const;
	tree_node_visit_t visit_decl;
	tree_node_visit_t visit_exp;
	tree_node_visit_t visit_id;
	tree_node_visit_t visit_list;
	tree_node_visit_t visit_stmt;
	tree_node_visit_t visit_typespec;
} tree_node_visitor_t;

void parser_init();
void parser_finit();
tree_t parser_file();
void parser_visit_tree(tree_node_visitor_t *visitor, tree_t tree);



extern tree_node_visitor_t print_visitor;
extern tree_node_visitor_t check_visitor;



void fatal(const char *fmt, ...);
void warn(const char *fmt, ...);
void fatal_tree(tree_t tree, const char *fmt, ...);
void warn_tree(tree_t tree, const char *fmt, ...);

#endif // JAVAC_H_INCLUDED

