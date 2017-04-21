#include <limits.h>

#define MAXV 1000

typedef struct edgenode{
  int y;
  int weight;
  struct edgenode *next;
} edgenode;

typedef struct {
  edgenode *edges[MAXV + 1];
  int degree[MAXV + 1];
  int nvertices;
  int nedges;
  int directed;
} graph;

void prim(graph *g, int start)
{
  int i; /* counter */
  edgenode *p; /*temporary pointer */
  bool intree[MAXV + 1]; //is the vertex in the tree yet
  int distance[MAXV + 1]; // cost of adding to tree
  int parent[MAXV + 1]; // parent of vertex
  int v;                 // current vertex to process
  int w;                // candidate next vertex
  int weight;          // edge weight
  int dist;            // best current distance from start

  for(i = 1; i <= g-> nvertices; i ++){
    intree[i] = false;
    distance[i] = INT_MAX;
    parent[i] = -1;
  }

  distance[start] = 0;
  v= start;

  while(intree[v] == false){
    intree[v] = true;
    p = g -> edges[v];
    while(p != NULL){
      w = p -> y;
      weight = p -> weight;
      if ((distance[w] > weight) && (intree[w] == false)){
	distance[w] = weight;
	parent[w] = v;
      }
      p = p -> next;
    }

    v = 1;
    dist = INT_MAX;

    for(i = 1; i <= g -> nvertices; i ++){
      if((intree[i] = false) && (dist > distance[i])){
	dist = distance[i];
	v = i;
      }
    }
  }
  
}
  
