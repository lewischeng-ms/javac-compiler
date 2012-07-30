#include "javac.h"

static void errout(int lineno, const char *kind, const char *fmt, va_list args)
{
	fprintf(stderr, "%s: ", kind);
	vfprintf(stderr, fmt, args);
	
	// We do not display line number until
	// it is set to be positive
	// after successfully opening the file.
	if (!lineno)
		fprintf(stderr, "\n");
	else
		fprintf(stderr, " (@%d)\n", lineno);
}

void fatal(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	errout(lineno, "fatal", fmt, args);
	va_end(args);
	exit(1);
}

void warn(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	errout(lineno, "warning", fmt, args);
	va_end(args);
}

void fatal_tree(tree_t tree, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	errout(TREE_NODE_LNO(tree), "fatal", fmt, args);
	va_end(args);
	exit(1);
}

void warn_tree(tree_t tree, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	errout(TREE_NODE_LNO(tree), "warning", fmt, args);
	va_end(args);
}

static void show_usage(const char *name)
{
	printf("usage: %s <java file>\n", name);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		show_usage(argv[0]);
		return 0;
	}
	
#	ifndef NDEBUG
	slist_unittest();
	hashtab_unittest();
#	endif
	
	// lex and parse.
	lex_init(argv[1]);
	// lex_print_all_tokens();
	parser_init();
	tree_t tree = parser_file();
	parser_finit();
	lex_finit();
	
#	ifndef NDEBUG
	parser_visit_tree(&print_visitor, tree);
#	endif
	
	// semantic check.
	parser_visit_tree(&check_visitor, tree);
	
	// gen IR.
	
#	ifndef NDEBUG
	// this function is useful for us
	// to detect memory leakage or wastage.
	xstat();
#	endif

	return 0;
}
