#define MAXV 1000

bool processed[MAXV + 1];
bool discovered[MAXV + 1];
int parent[MAXV + 1];
int entry_time[MAXV + 1];
int exit_time[MAXV + 1];

bool finished;
int time;

typedef struct edgenode{
  int y;
  int weight;
  struct edgenode *next;
} edgenode;

typedef struct graph{
  edgenode *edges[MAXV + 1];
  int degree[MAXV + 1];
  int nvertices;
  int nedges;
  bool directed;
} graph;

void initialize_graph(graph *g, bool directed);
void insert_edge(graph *g, int x, int y, bool directed);
void read_graph(graph *g, bool directed);
void print_graph(graph *g);
void process_vertex_late(int v);
void process_vertex_early(int v);
void process_edge(int x, int y);
void bfs(graph *g, int start);
void find_path(int start, int end, int parents[]);
void dfs(graph *g, int v);
