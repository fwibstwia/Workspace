#include <stdio.h>
#include <stdlib.h>

#define item_type int
#define PQ_SIZE 16

typedef struct{
  item_type q[PQ_SIZE + 1];
  int n;
} priority_queue;

int pq_parent(int n){
  if (n == 1)
    return -1;
  else
    return (int) n/2;
}

int pq_young_child(int n){
  return n * 2;
}

void bubble_up(priority_queue *q, int p)
{
  if(pq_parent(p) == -1){
    return;
  }

  if(q -> q[pq_parent(p)] > q -> q[p]){
    item_type temp = q -> q[pq_parent(p)];
    q -> q[pq_parent(p)] = q -> q[p];
    q -> q[p] = temp;
    bubble_up(q, pq_parent(p));
  }
}

void pq_insert(priority_queue *q, item_type x)
{
  if(q -> n >= PQ_SIZE){
    printf("Warning: priority queue overflow insert x=%d. \n", x);
    return;
  }

  q -> q[q -> n + 1] = x;
  q -> n = q -> n + 1;
  bubble_up(q, q -> n);  
}

void pq_init(priority_queue *q)
{
  q -> n = 0;
}

void make_heap(priority_queue *q, item_type s[], int n)
{
  int i;
  pq_init(q);

  for(i = 0; i < n; i ++)
    pq_insert(q, s[i]);
}

void bubble_down(priority_queue *q, int p)
{
  int c;
  int i;
  int min_index;
  c = pq_young_child(p);
  min_index = p;

  for(i = 0; i <= 1; i ++)
    if((c + i) <= q -> n){
      if(q -> q[min_index] > q -> q[c + i])
	min_index = c + i;
    }

  if(min_index != p){
    int temp = q -> q[p];
    q -> q[p] = q -> q[min_index];
    q -> q[min_index] = temp;
    bubble_down(q, min_index);
  }
}


item_type extract_min(priority_queue *q)
{
  int min = -1;
  if((q -> n) <= 0){
    printf("Warning: empty priority queue.\n");
  }else{
    min = q -> q[1];
    q -> q[1] = q -> q[q -> n];
    q -> n = q -> n - 1;
    bubble_down(q, 1);
  }
  return min;
}


int main()
{
  priority_queue q;
  int s[5] = {2, 5,3,4,1};
  make_heap(&q, s, 5);

  for(int i = 0; i < 5; i ++){
    s[i] = extract_min(&q);
    printf("%d \n", s[i]);
  }
  
}
