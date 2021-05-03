#include <stdio.h>
#include "avl.h"

void
print(avl_key_t k)
{
	printf("\t%d", k);
}

int
main(void)
{
	avl_node t = NULL;

	t = avl_insert(t, 23);
	t = avl_insert(t, 22);
	t = avl_insert(t, 15);
	t = avl_insert(t, 16);
	t = avl_insert(t, 4);
	t = avl_insert(t, 42);
	t = avl_insert(t, 8);
	t = avl_delete(t, 22);

	printf("Min.:\t%d\n", avl_min(t));
	printf("Max.:\t%d\n", avl_max(t));
	printf("Keys:");
	avl_traverse(t, &print);
	printf("\n");
	printf("Pred.(15):\t%d\n", avl_predecessor(t, 15));
	printf("Succ.(17):\t%d\n", avl_successor(t, 17));
}
