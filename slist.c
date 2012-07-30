#include "javac.h"

slist_t slist_create()
{
	pslist_node_t phead = xmalloc(sizeof(slist_node_t));
	phead->plink = NULL;
	
	return phead;
}

void slist_destroy(slist_t list)
{
	pslist_node_t pnode = list;
	while (pnode) {
		pslist_node_t pnext = pnode->plink;
		xfree(pnode);
		pnode = pnext;
	}
}

void slist_iter_begin(slist_t list, pslist_iter_t piter)
{
	assert(list);
	assert(piter);
	
	piter->plast = list;
	piter->pcurr = list->plink;
}

void slist_iter_move_next(pslist_iter_t piter)
{
	assert(piter);
	assert(piter->pcurr);
	
	piter->plast = piter->pcurr;
	piter->pcurr = piter->pcurr->plink;
}

bool slist_iter_not_end(pslist_iter_t piter)
{
	assert(piter);
	
	return piter->pcurr != NULL;
}

void *slist_iter_deref(pslist_iter_t piter)
{
	assert(piter);
	assert(piter->pcurr);
	
	return (void *)piter->pcurr->data;
}

void slist_insert(pslist_iter_t piter, const void *pdata, size_t size)
{
	assert(piter);
	
	pslist_node_t pnode = xmalloc(sizeof(slist_node_t) + size);
	memcpy(pnode->data, pdata, size);
	pnode->plink = piter->pcurr;
	piter->plast->plink = pnode;
	piter->pcurr = pnode;
}

void slist_push_front(slist_t list, const void *pdata, size_t size)
{
	assert(list);
	
	slist_iter_t iter;
	slist_iter_begin(list, &iter);
	slist_insert(&iter, pdata, size);
}

void slist_remove(pslist_iter_t piter)
{
	assert(piter);
	assert(piter->pcurr);
	
	piter->plast->plink = piter->pcurr->plink;
	pslist_node_t pnext = piter->pcurr->plink;
	xfree(piter->pcurr);
	piter->pcurr = pnext;
}

bool slist_is_empty(slist_t list)
{
	assert(list);
	
	return list->plink == NULL;
}

void slist_unittest()
{
	slist_t list = slist_create();
	slist_iter_t iter;
	slist_iter_begin(list, &iter);
	
	int i;
	for (i = 1; i <= 100; ++i)
		slist_insert(&iter, &i, sizeof(int));
	
	while (slist_iter_not_end(&iter)) {
		slist_remove(&iter);
		slist_iter_move_next(&iter);
	}

	i = 99;
	for (slist_iter_begin(list, &iter); slist_iter_not_end(&iter); slist_remove(&iter)) {
		assert(i == SLIST_ITER_GET_INT(iter));
		i -= 2;
	}
	
	assert(slist_is_empty(list));
	
	slist_destroy(list);
	
	printf("test slist ok\n");
}