#include "javac.h"

static int n_malloc;
static int n_free;

void *xmalloc(size_t size)
{
	void *p = malloc(size);
	
	if (!p)
		fatal("not enough memory");
		
	n_malloc++;
	
	return p;
}

void xfree(void *p)
{
	if (!p)
		return;
		
	free(p);
	
	n_free++;
}

char *xstrdup(const char *src)
{
	assert(src);
	
	size_t len = strlen(src);
	char *dst = xmalloc(len + 1);
	return strcpy(dst, src);
}

void xstat()
{
	printf("stats for memory usage: %d allocations, %d deallocations.\n", n_malloc, n_free);
}