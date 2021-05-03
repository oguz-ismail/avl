#include <assert.h>
#include <stdlib.h>

#define avl_key_t int
#define keyeq(x, y) (x == y)
#define keylt(x, y) (x < y)
#define keygt(x, y) (x > y)
#define avl_assert assert
#define avl_malloc malloc
#define avl_free free

enum avl_balance {
	BALANCED,
	LEFT_HEAVY,
	RIGHT_HEAVY,
	AVL_VIOLATIVE,
	DOUBLY_LEFT_HEAVY,
	DOUBLY_RIGHT_HEAVY,
	INVALID
};

struct avl_node {
	avl_key_t key;
	struct avl_node *left;
	struct avl_node *right;
	size_t height;
	enum avl_balance balance;
};

static struct avl_node *rebalance(struct avl_node *);

static size_t
height(of)
	struct avl_node *of;
{
	if (!of->left && !of->right)
		return 0;
	if (!of->left)
		return of->right->height + 1;
	if (!of->right)
		return of->left->height + 1;
	if (of->left->height > of->right->height)
		return of->left->height + 1;
	return of->right->height + 1;
}

static ssize_t
imbalance(of)
	struct avl_node *of;
{
	if (!of->left && !of->right)
		return 0;
	if (!of->left)
		return of->right->height + 1;
	if (!of->right)
		return -(of->left->height + 1);
	return of->right->height - of->left->height;
}

static enum avl_balance
balance(of)
	struct avl_node *of;
{
	switch (imbalance(of)) {
		case  0: return BALANCED;
		case -1: return LEFT_HEAVY;
		case  1: return RIGHT_HEAVY;
		case -2: return DOUBLY_LEFT_HEAVY;
		case  2: return DOUBLY_RIGHT_HEAVY;
		default: return INVALID;
	}
}

static void
update(node)
	struct avl_node *node;
{
	node->height  = height(node);
	node->balance = balance(node);
}

static struct avl_node *
insertnode(in, node)
	struct avl_node *in, *node;
{
	if (!in)
		return node;
	if (!node)
		goto end;
	if (keyeq(node->key, in->key)) {
		in = insertnode(in, node->left);
		in = insertnode(in, node->right);
		avl_free(node);
		return in;
	}
	if (keylt(node->key, in->key))
		in->left = insertnode(in->left, node);
	else if (keygt(node->key, in->key))
		in->right = insertnode(in->right, node);
#if avl_debug
	else
		/* XXX show an error message */
		avl_abort();
#endif
end:
	update(in);
	if (in->balance >= AVL_VIOLATIVE)
		in = rebalance(in);
	return in;
}

static struct avl_node *
rotateleft(node)
	struct avl_node *node;
{
	struct avl_node *right;

	right = node->right;
	node->right = NULL;
	node = insertnode(node, right->left);
	right->left = NULL;
	return insertnode(right, node);
}

static struct avl_node *
rotateright(node)
	struct avl_node *node;
{
	struct avl_node *left;
	left = node->left;
	node->left = NULL;
	node = insertnode(node, left->right);
	left->right = NULL;
	return insertnode(left, node);
}

static struct avl_node *
rebalance(tree)
	struct avl_node *tree;
{
	/* TODO */
	switch (tree->balance) {
		case DOUBLY_LEFT_HEAVY:
			if (tree->left->balance == RIGHT_HEAVY)
				tree->left = rotateleft(tree->left);
			return rotateright(tree);
		case DOUBLY_RIGHT_HEAVY:
			if (tree->right->balance == LEFT_HEAVY)
				tree->right = rotateright(tree->right);
			return rotateleft(tree);
	}
}

struct avl_node *
avl_insert(in, key)
	struct avl_node *in;
	avl_key_t key;
{
	struct avl_node *new;

	new = avl_malloc(sizeof(struct avl_node));
	new->key = key;
	new->left = NULL;
	new->right = NULL;
	new->height = 0;
	new->balance = BALANCED;
	return insertnode(in, new);
}

struct avl_node *
avl_delete(from, key)
	struct avl_node *from;
	avl_key_t key;
{
	if (!from)
		return from;
	if (keyeq(key, from->key))
		return insertnode(from->left, from->right);
	if (keylt(key, from->key))
		from->left = avl_delete(from->left, key);
	else if (keygt(key, from->key))
		from->right = avl_delete(from->right, key);
	return insertnode(from, NULL);
}

int
avl_search(in, key)
	struct avl_node *in;
	avl_key_t key;
{
	if (!in)
		return 0;
	if (keyeq(key, in->key))
		return 1;
	if (keylt(key, in->key))
		return avl_search(in->left, key);
	if (keygt(key, in->key))
		return avl_search(in->right, key);
#if avl_debug
	avl_abort();
#endif
}

avl_key_t
avl_min(in)
	struct avl_node *in;
{
	avl_assert(in != NULL);
	for (; in->left; in = in->left);
	return in->key;
}

avl_key_t
avl_max(in)
	struct avl_node *in;
{
	avl_assert(in != NULL);
	for (; in->right; in = in->right);
	return in->key;
}

int
main(void)
{
	struct avl_node *x;

	x = NULL;

	for (int i = 0; i < 100; i++) 
		x = avl_insert(x, rand());
}
