//red-black properties
// a. Every node is either red or black
// b. The root is black
// c. Every leaf(NIL) is black
// d. If a node is red, then both its children are black
// e. For each node, all paths from the node to descendant leaves contain the same number of black nodes.
class Tree{
  int key;
  Tree* left;
  Tree* right;
public:
  Tree* search(int k);
  Tree* minimum();
  Tree* maximum();
  Tree* successor();
  Tree* predecessor();
};


class RedBlackTree:public Tree{
private:
  int key;
  bool status;
public:
  RedBlackTree(int k):key(k), status(true){};
  RedBlackTree():status(false){};
  RedBlackTree* search(int key);
};

Tree* Tree::search(int k){
  Tree* p = this;
  while(p != NULL && p.key != k){
    if(k < p.key){
      p = p -> left;
    }else{
      p = p -> right;
    }
  }
  return p;
}

Tree* Tree::minimum(){
  Tree* p = this;
  while(p -> left != NULL){
    p = p -> left;
  }
  return p;  
}

Tree* Tree::maximum(){
  Tree* p = this;
  while(p -> right != NULL){
    p = p -> right;
  }
}

Tree* Tree::successor
