#include <assert.h>
#include <stdlib.h>

/* type of keys to be stored in AVL nodes. */
#define avl_key_t int
/* queries that return a key will return this
 * value when no result is available. */
#define AVL_SENTINEL -1
/* functions/macros to be used for comparing keys.
 * these should take two parameters of whatever
 * type "avl_key_t" defined to be. */
#define avl_keylt(x, y) (x < y)
#define avl_keygt(x, y) (x > y)
/* assert-like macro. */
#define avl_assert assert
/* function/macro for releasing memory
 * previously allocated for AVL nodes.
 * should take one parameter of type
 * "void *". */
#define avl_free free
/* function/macro for allocating memory for
 * new AVL nodes. should take one parameter
 * of type "size_t". */
#define avl_malloc malloc
/* turn on/off debug mode. */
#define avl_debug 1

/* include necessary headers at the top of this
 * file, do not edit "avl.c". */

typedef struct avl_node *avl_node;

int avl_search(avl_node, avl_key_t);
avl_key_t avl_min(avl_node);
avl_key_t avl_max(avl_node);
avl_key_t avl_predecessor(avl_node, avl_key_t);
avl_key_t avl_successor(avl_node, avl_key_t);
avl_node avl_insert(avl_node, avl_key_t);
avl_node avl_delete(avl_node, avl_key_t);
void avl_traverse(avl_node, void (*)(avl_key_t));
