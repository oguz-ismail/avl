#include "avl.h"

#define max(x, y) (x > y ? x : y)
#define height(n) (n ? n->height : -1)
#define newheight(n) \
	(max(height(n->left), height(n->right)) + 1)

typedef enum {
	BALANCED,
	LEFT_HEAVY,
	RIGHT_HEAVY,
	AVL_VIOLATIVE,
	DOUBLY_LEFT_HEAVY,
	DOUBLY_RIGHT_HEAVY,
	AVL_INVALID,
} avl_status;

struct avl_node {
	avl_key_t key;
	struct avl_node *left;
	struct avl_node *right;
	int height;
	avl_status status;
};

static avl_node rebalance(avl_node);

/*
 * calculate how much children of "t" differ in
 * height, and translate the result to "avl_status".
 */
static avl_status
newstatus(t)
	avl_node t;
{
	switch (height(t->right) - height(t->left)) {
		case  0: return BALANCED;
		case -1: return LEFT_HEAVY;
		case  1: return RIGHT_HEAVY;
		case -2: return DOUBLY_LEFT_HEAVY;
		case  2: return DOUBLY_RIGHT_HEAVY;
		default: return AVL_INVALID;
	}
}

/*
 * rotate node "t" to its left.
 */
static avl_node
rotateleft(t)
	avl_node t;
{
	avl_node n;

#if avl_debug
	avl_assert(t->right != NULL);
#endif
	n = t->right;
	t->right = n->left;
	n->left = rebalance(t);
	return rebalance(n);
}

/*
 * rotate right.
 */
static avl_node
rotateright(t)
	avl_node t;
{
	avl_node n;

#if avl_debug
	avl_assert(t->left != NULL);
#endif
	n = t->left;
	t->left = n->right;
	n->right = rebalance(t);
	return rebalance(n);
}

/*
 * recover from insertion, deletion, or rotation.
 */
static avl_node
rebalance(t)
	avl_node t;
{
#if avl_debug
	avl_assert(t != NULL);
#endif
	t->height = newheight(t);
	t->status = newstatus(t);
#if avl_debug
	avl_assert(t->status < AVL_INVALID);
#endif
	switch (t->status) {
		case DOUBLY_LEFT_HEAVY:
#if avl_debug
			avl_assert(t->left != NULL);
#endif
			if (t->left->status == RIGHT_HEAVY)
				t->left = rotateleft(t->left);
			t = rotateright(t);
			break;
		case DOUBLY_RIGHT_HEAVY:
#if avl_debug
			avl_assert(t->right != NULL);
#endif
			if (t->right->status == LEFT_HEAVY)
				t->right = rotateright(t->right);
			t = rotateleft(t);
			break;
	}
	return t;
}

/*
 * return whether key "k" exists in tree "t".
 */
int
avl_search(t, k)
	avl_node t;
	avl_key_t k;
{
	for (;;) {
		if (!t)
			return 0;
		if (avl_keylt(k, t->key))
			t = t->left;
		else if (avl_keygt(k, t->key))
			t = t->right;
		else
			return 1;
	}
}

/*
 * return the least key stored in tree "t".
 */
avl_key_t
avl_min(t)
	avl_node t;
{
	if (!t)
		return AVL_SENTINEL;
	for (; t->left; t = t->left);
	return t->key;
}

/*
 * return the greatest.
 */
avl_key_t
avl_max(t)
	avl_node t;
{
	if (!t)
		return AVL_SENTINEL;
	for (; t->right; t = t->right);
	return t->key;
}

/*
 * return the greatest key stored in tree
 * "t" that is less than key "k".
 */
avl_key_t
avl_predecessor(t, k)
	avl_node t;
	avl_key_t k;
{
	avl_node n;

	n = t;
	t = NULL;
	while (n) {
		if (avl_keylt(k, n->key))
			n = n->left;
		else if (avl_keygt(k, n->key)) {
			t = n;
			n = n->right;
		}
		else {
			if (!n->left)
				break;
			else
				return avl_max(n->left);
		}
	}
	if (!t)
		return AVL_SENTINEL;
	else
		return t->key;
}

/*
 * return the least greater than "k".
 */
avl_key_t
avl_successor(t, k)
	avl_node t;
	avl_key_t k;
{
	avl_node n;

	n = t;
	t = NULL;
	while (n) {
		if (avl_keylt(k, n->key)) {
			t = n;
			n = n->left;
		}
		else if (avl_keygt(k, n->key))
			n = n->right;
		else {
			if (!n->right)
				break;
			else
				return avl_min(n->right);
		}
	}
	if (!t)
		return AVL_SENTINEL;
	else
		return t->key;
}

/*
 * insert key "k" into tree "t".
 */
avl_node
avl_insert(t, k)
	avl_node t;
	avl_key_t k;
{
	avl_node n;

	if (!t) {
		n = avl_malloc(sizeof(struct avl_node));
		n->key = k;
		n->left = NULL;
		n->right = NULL;
		n->height = 0;
		n->status = BALANCED;
		return n;
	}
#if avl_debug
	avl_assert(t->status < AVL_VIOLATIVE);
#endif
	if (avl_keylt(k, t->key))
		t->left = avl_insert(t->left, k);
	else if (avl_keygt(k, t->key))
		t->right = avl_insert(t->right, k);
	else	/* key exists */
		return t;
	return rebalance(t);
}

/*
 * delete key "k" from tree "t".
 */
avl_node
avl_delete(t, k)
	avl_node t;
	avl_key_t k;
{
	avl_node n;

	if (!t)
		return t;
#if avl_debug
	avl_assert(t->status < AVL_VIOLATIVE);
#endif
	if (avl_keylt(k, t->key))
		t->left = avl_delete(t->left, k);
	else if (avl_keygt(k, t->key))
		t->right = avl_delete(t->right, k);
	else {
		if (!t->left)
			n = t->right;
		else if (!t->right)
			n = t->left;
		else {
			/* replace "t" with its in-order successor. */
			k = avl_min(t->right);
			n = avl_insert(NULL, k);
			n->left = t->left;
			n->right = avl_delete(t->right, k);
			n = rebalance(n);
		}
		avl_free(t);
		return n;
	}
	return rebalance(t);
}

/*
 * perform an in-order traversal of tree "t",
 * calling "f" for each key.
 */
void
avl_traverse(t, f)
	avl_node t;
	void (*f)(avl_key_t);
{
	if (!t)
		return;
	avl_traverse(t->left, f);
	f(t->key);
	avl_traverse(t->right, f);
}
