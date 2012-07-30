#include "javac.h"

phashtab_t hashtab_create(int n_buckets)
{
	phashtab_t hashtab = xmalloc(sizeof(hashtab_t));
	hashtab->n_elems = 0;
	
	hashtab->n_buckets = n_buckets;
	hashtab->buckets = xmalloc(hashtab->n_buckets * sizeof(slist_t));
	int i;
	for (i = 0; i != hashtab->n_buckets; ++i)
		hashtab->buckets[i] = slist_create();
		
	return hashtab;
}

void hashtab_destroy(phashtab_t phashtab)
{
	assert(phashtab);
	
	int i;
	for (i = 0; i != phashtab->n_buckets; ++i) {
		// Free the spaces for 'key'.
		slist_iter_t iter;
		slist_iter_begin(phashtab->buckets[i], &iter);
		while (slist_iter_not_end(&iter)) {
			pair_t pair = SLIST_ITER_GET_T(iter, pair_t);
			assert(pair.key);
			xfree((void *)pair.key);
			slist_iter_move_next(&iter);
		}
		
		// Destroy the bucket.
		slist_destroy(phashtab->buckets[i]);
	}
		
	xfree(phashtab->buckets);
	xfree(phashtab);
}

static int string_hash(const char *key)
{
	int hash = 0;
 
	while (*key) {
		hash = (hash << 4) + (*key++);
		
		int temp = hash & 0xF0000000;
		if (temp) {
			hash = hash ^ (temp >> 24);
			hash = hash ^ temp;
		}
	}
 
	return hash & 0x7FFFFFFF;
}

void hashtab_insert(phashtab_t phashtab, const char *key, const void *pdata, size_t size)
{
	assert(phashtab);
	assert(key);
	
	int hash = string_hash(key);
	int index = hash % phashtab->n_buckets;
	
	// Create a pair.
	size_t pair_size = sizeof(pair_t) + size;
	ppair_t ppair = xmalloc(pair_size);
	ppair->key = xstrdup(key);
	if (pdata && size > 0)
		memcpy(ppair->data, pdata, size);
	
	// Insert to that bucket.
	slist_iter_t iter;
	slist_iter_begin(phashtab->buckets[index], &iter);
#	ifndef NDEBUG
	// Ensure no duplicate key in the same hash table.
	while (slist_iter_not_end(&iter)) {
		ppair_t ppair = &SLIST_ITER_GET_T(iter, pair_t);
		assert(strcmp(ppair->key, key));
		slist_iter_move_next(&iter);
	}
#	endif
	slist_insert(&iter, ppair, pair_size);
	
	xfree(ppair);
}

bool hashtab_lookup(phashtab_t phashtab, const char *key, void *pdata, size_t size)
{
	assert(phashtab);
	assert(key);
	
	int hash = string_hash(key);
	int index = hash % phashtab->n_buckets;
	
	slist_iter_t iter;
	slist_iter_begin(phashtab->buckets[index], &iter);
	while (slist_iter_not_end(&iter)) {
		ppair_t ppair = &SLIST_ITER_GET_T(iter, pair_t);
		
		if (!strcmp(ppair->key, key)) {
			if (pdata && size > 0)
				memcpy(pdata, ppair->data, size);
			
			return true;
		}
		
		slist_iter_move_next(&iter);
	}
	
	return false;
}

void hashtab_stat(phashtab_t phashtab, const char *name)
{
	printf("stat for hashtab '%s': \n", name);
	int sum = 0;
	int empty = 0;
	int i;
	for (i = 0; i != phashtab->n_buckets; ++i) {
		int local_sum = 0;
		slist_iter_t iter;
		slist_iter_begin(phashtab->buckets[i], &iter);
		while (slist_iter_not_end(&iter)) {
			local_sum++;
			sum++;
			slist_iter_move_next(&iter);
		}
#		if HASHTAB_STAT_VERBOSE
		printf("%d elements in bucket %d\n", local_sum, i);
#		endif
		
		if (slist_is_empty(phashtab->buckets[i]))
			empty++;
	}
	printf("element count = %d, load factor = %.2f, bucket util = %.2f\n",
		sum,
		(double)sum / phashtab->n_buckets,
		1.0 - (double)empty / phashtab->n_buckets
  		);
}

void hashtab_unittest()
{
	static char *strs[] = {
		"hello", "word", "native", "java",
		"int", "char", "<=", ">=", "==",
		"for", "if", "while", "else",
		NULL
	};
	
	static char *strs1[] = {
		"oracle", "microsoft", "lava",
		NULL
	};
	
	phashtab_t pht = hashtab_create(31);
	
	char **str = strs;
	while (*str) {
		hashtab_insert(pht, *str, *str, strlen(*str) + 1);
		str++;
	}
	
#	ifndef NDEBUG
	char temp[10];
#	endif
	str = strs;
	while (*str) {
		assert(hashtab_lookup(pht, *str, temp, strlen(*str) + 1));
		assert(!strcmp(*str, temp));
		str++;
	}
	
	str = strs1;
	while (*str) {
		assert(!hashtab_lookup(pht, *str, NULL, 0));
		str++;
	}
	
	hashtab_destroy(pht);
	
	printf("test hashtab ok\n");
}