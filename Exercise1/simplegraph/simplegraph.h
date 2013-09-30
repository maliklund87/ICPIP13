#ifndef _simple_graph_
#define _simple_graph_

#include <math/f4x4.h>

#ifdef __cplusplus
extern "C" {
#endif

#define simple_graph_max_children 4

	// typedef struct node node;
	// typedef struct node{
	// 	f4x4 transform;
	// 	unsigned int object_no;
	// 	unsigned int children_count;
	// 	node* children[simple_graph_max_children];
	// } node;

	struct node{
		f4x4 transform;
		unsigned int object_no;
		unsigned int children_count;
		node* children[simple_graph_max_children];
	};

	node* make_node();
	void destroy_node(node *n);
	void destroy_node_rec(node *n);
	void add_child(node *n, node *child);

#ifdef __cplusplus
}
#endif

#endif /* _simple_graph_ */
