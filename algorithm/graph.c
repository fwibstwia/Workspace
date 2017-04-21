#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

void initialize_graph(graph *g, bool directed)
{
  int i;
  g -> nvertices = 0;
  g -> nedges = 0;
  g -> directed = directed;

  for(i = 1; i <= MAXV; i ++)
    g -> degree[i] = 0;
  for(i = 1; i <= MAXV; i ++)
    g -> edges[i] = NULL;
}

void insert_edge(graph *g, int x, int y, bool directed)
{
  edgenode *p;
  p = (edgenode *)malloc(sizeof(edgenode));

  p -> weight = -1;
  p -> y = y;
  p -> next = g -> edges[x];
  g -> edges[x] = p;
  g -> degree[x] ++;
  if(directed == false)
    insert_edge(g, y, x, true);
  else
    g -> nedges ++;
}

void read_graph(graph *g, bool directed)
{
  int i;
  int m;
  int x, y;
  initialize_graph(g, directed);
  scanf("%d %d", &(g -> nvertices), &m);

  for(i = 1; i <= m; i ++){
    scanf("%d %d", &x, &y);
    insert_edge(g, x, y, directed);
  }
}

void print_graph(graph *g)
{
  int i;
  edgenode *p;
  
}

void initialized_search(graph *g)
{
  for(int i = 1; i <= g -> nvertices; i ++){
    processed[i] = false;
    discovered[i] = false;
    parent[i] = -1;
  }
}

void process_vertex_late(int v)
{
}

void process_vertex_early(int v)
{
  printf("processed vertex %d\n", v);
}

void process_edge(int x, int y)
{
  printf("processed edge (%d, %d)\n", x, y);
}

void bfs(graph *g, int start)
{
  queue q;
  int v;
  int y;
  edgenode *p;

  init_queue(&q);
  enqueue(&q, start);
  discovered[start] = true;

  while(!empty_queue(&q)){
    v = dequeue(&q);
    process_vertex_early(v);
    p = g -> edges[v];
    while(p != NULL){
      if((processed[y] = false) || g -> directed)
	 process_edge(v, y);
      if(discovered[p -> y] == false){
	discovered[p -> y] = true;
	parent[p -> y] = v;
	enqueue(&q, p -> y);
      }
      p = p -> next;
    }
    processed[v] = true;
  } 
}

void find_path(int start, int end, int parents[])
{
  if((start == end) || (end == -1))
    printf("\n%d", start);
  else{
    find_path(start, parents[end], parents);
    printf(" %d", end);
  }
}

void dfs(graph *g, int v)
{
  edgenode *p;
  int y;

  if(finished) return;

  discovered[v] = true;
  time = time + 1;
  entry_time[v] = time;

  process_vertex_early(v);
  
  p = g -> edges[v];
  while(p != NULL){
    y = p -> y;
    if(discovered[y] == false){
      parent[y] = v;
      process_edge(v, y);
      dfs(g, y);
    }else if((!processed[y]) || g -> directed){
      process_edge(v, y);
    }
    if(finished) return;
    p = p -> next;
  }

  time = time + 1;
  exit_time[v] = time;
  processed[v] = true;
}


