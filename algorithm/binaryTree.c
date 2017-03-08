#include <stdio.h>
#include <stdlib.h>

#define item_type int

typedef struct tree {
  item_type item;
  struct tree *parent;
  struct tree *left;
  struct tree *right;
} tree;

void process_item(item_type item){
  
}

tree *search_tree(tree *l, item_type x){
  if( l == NULL || l -> item == x){
    return l;
  }

  if(x < l -> item){
    return search_tree(l -> left, x);
  }else{
    return search_tree(l -> right, x);
  }
}

tree *find_minimum(tree *t)
{
  if(t == NULL){
    return t;
  }

  tree *min;
  min = t;
  while(min -> left != NULL)
    min = min -> left;
  return min;
}

void traverse_tree(tree *l)
{
  if(l != NULL){
    traverse_tree(l -> left);
    process_item(l -> item);
    traverse_tree(l -> right);
  }
}

void insert_tree(tree **l, item_type x, tree *parent)
{
  tree *p;
  if(*l == NULL){
    p = (tree *) malloc(sizeof(tree));
    p -> item = x;
    p -> left = p -> right = NULL;
    p -> parent = parent;
    *l = p;
  } else if( x < (*l) -> item){
    insert_tree(&((*l) -> left), x, *l);
  } else{
    insert_tree(&((*l) -> right), x, *l);
  }
}
