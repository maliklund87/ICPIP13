#include "simplegraph.h"

#include <stdlib.h>

node* make_node(){
	node* n = (node*) malloc(sizeof(node));
	n->children_count=0;
	return n;
};


void destroy_node(node *n){
	free(n);
};

void destroy_node_rec(node *n){
	for(unsigned int i = 0; i < n->children_count; i++)
		destroy_node_rec(n->children[i]);
	free(n);
};


void add_child(node *n, node *child){
	if(n->children_count < simple_graph_max_children -1){
		n->children[n->children_count] = child;
		n->children_count+=1;
	}
};

