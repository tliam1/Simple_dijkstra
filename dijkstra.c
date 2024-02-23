#include <stdlib.h>
#include <stdio.h>

#define INFINITY 10000
#define NO_PRED -1

struct adjlist_node { // Node in the adjacency list
     int node_id;
     int weight;  // Weight of edge that goes to the node
     struct adjlist_node * next;
};

struct graph { // Nodes of the graph are 0, 1, ..., num_nodes-1
     int num_nodes;
     struct adjlist_node ** adjlist; // Adjacency list
};

struct algm_node {
     int dist;  // Estimated distance from the source node
     int pred;  // Parent node in the shortest path tree
};

struct algm_data {
     int num_nodes;
     struct algm_node * node;
};

struct priorityq {
     int length;
     int * node;
     int * in_q;
     int * dist; //dist corresponding to each node
};

/* Dijkstra's algorithm and managing its data structures */
struct algm_data * algm_data_create(struct graph * g, int source);
void algm_data_destroy(struct algm_data * a);
void algm_data_display(struct algm_data * a);
struct algm_data * dijkstra(struct graph * g, int source);

/* Priority queue for Dijkstra's aglrothm */
struct priorityq * priorityq_create(struct algm_data *a);
void priorityq_destroy(struct priorityq * pq);
int priorityq_not_empty(struct priorityq * pq);
int priorityq_get(struct priorityq* pq, struct algm_data * a);
int priorityq_in_q(struct priorityq * pq, int node);
void priorityq_update(struct priorityq* pq, struct algm_data * a, int node);

/* Managing graphs */
struct graph * graph_create(int num_nodes);
void graph_destroy(struct graph * g);
void graph_insert_link(struct graph *g, int head_node, int tail_node, int weight);
void graph_display(struct graph * g);
struct graph * create_graph0();
struct graph * create_graph1();

void main()
{
printf("Graph 0:\n");
struct graph * g = create_graph0();
graph_display(g);

printf("Output of Dijkstra's algorithm\n");
struct algm_data * a = dijkstra(g, 0);
algm_data_display(a);

algm_data_destroy(a);
graph_destroy(g);
printf("\n");

printf("Graph 1:\n");
g = create_graph1();
graph_display(g);

printf("Output of Dijkstra's algorithm\n");
a = dijkstra(g, 0);
algm_data_display(a);

algm_data_destroy(a);
graph_destroy(g);
}


struct algm_data * algm_data_create(struct graph * g, int source)
{
struct algm_data * a = (struct algm_data *) malloc(sizeof(struct algm_data));
a->num_nodes = g->num_nodes;

a->node = (struct algm_node *) malloc(sizeof(struct algm_node)*a->num_nodes);
for (int i=0; i<a->num_nodes; i++) {
    if (i != source) {
	    a->node[i].dist = INFINITY;
    }
    else {
	    a->node[i].dist = 0;
    }
    a->node[i].pred = NO_PRED;
};

return a;
}

void algm_data_destroy(struct algm_data * a)
{
free(a->node);
free(a);
}

struct priorityq * priorityq_create(struct algm_data *a)
{
struct priorityq * pq = (struct priorityq *) malloc(sizeof(struct priorityq));

pq->node = (int *) malloc(sizeof(int)*a->num_nodes);
pq->in_q = (int *) malloc(sizeof(int)*a->num_nodes);
pq->length =0;
pq->dist = (int *) malloc(sizeof(int)*a->num_nodes); 
for (int i=0; i<a->num_nodes; i++) {
     pq->node[pq->length] = i;
     pq->in_q[pq->length] = 1;
     pq->dist[i] = INFINITY;
     pq->length++;
}





return pq;
}

void priorityq_destroy(struct priorityq * pq)
{
free(pq->node);
free(pq->in_q);
free(pq);
}

int priorityq_not_empty(struct priorityq * pq)
{
if (pq->length > 0) return 1;
else return 0;
}

int priorityq_in_q(struct priorityq * pq, int node)
{
return pq->in_q[node];
}


int priorityq_get(struct priorityq* pq, struct algm_data * a)
{
   if (pq->length <= 0)
      return -1;

   /* Find the nearest node */
   int min_node = pq->node[0];
   int min_dist = pq->dist[min_node];
   int min_index = 0;

   for (int i = 1; i < pq->length; i++) {
      if (pq->dist[pq->node[i]] < min_dist) {
         min_dist = pq->dist[pq->node[i]];
         min_node = pq->node[i];
         min_index = i;
      }
   }

   int last_node = pq->node[pq->length - 1];
   pq->node[min_index] = last_node;
   pq->in_q[min_node] = 0;
   pq->length--;

   // heapify
   // formulas: children --> 2 * i + (1 (left) and 2 (right))
   //
   int i = min_index;
   while (1) {
      int left_child = 2 * i + 1;
      int right_child = 2 * i + 2;
      int smallest = i;

      //if either child is smaller than smallest-->swap
      //
      if (left_child < pq->length && pq->dist[pq->node[left_child]] < pq->dist[pq->node[smallest]])
         smallest = left_child;

      if (right_child < pq->length && pq->dist[pq->node[right_child]] < pq->dist[pq->node[smallest]])
         smallest = right_child;
      //if swaps where made update i
      if (smallest != i) {
         int temp = pq->node[i];
         pq->node[i] = pq->node[smallest];
         pq->node[smallest] = temp;
         i = smallest;
      } else {
         break;
      }
   }

   return min_node;
}

void priorityq_update(struct priorityq* pq, struct algm_data * a, int node)
{

   //goal: ensure the node with the smallest distance is at the top of the priority queue

   // find node in queue
   int index = -1;
   for (int i = 0; i < pq->length; i++) {
      if (pq->node[i] == node) {
         index = i;
         break;
      }
   }

   if (index == -1)
      return;  // not found

   pq->dist[node] = a->node[node].dist;

   // heapify
   int i = index;
   while (1) {
      int parent = (i - 1) / 2;
      if (parent >= 0 && pq->dist[pq->node[i]] < pq->dist[pq->node[parent]]) {
         int temp = pq->node[i];
         pq->node[i] = pq->node[parent];
         pq->node[parent] = temp;
         i = parent;
      } else {
         break;
      }
   }
   return;
}

struct graph * graph_create(int num_nodes)
{
struct graph * g = (struct graph *) malloc(sizeof(struct graph));
g->num_nodes = num_nodes;
g->adjlist = (struct adjlist_node **) malloc(sizeof(struct adjlist_node *)*num_nodes);
for (int i=0; i<num_nodes; i++) {
     g->adjlist[i] = NULL;
}

return g;
}

void graph_destroy(struct graph * g)
{
for (int i=0; i<g->num_nodes; i++) {
     for (struct adjlist_node * p = g->adjlist[i]; p != NULL;) {
     	struct adjlist_node * temp = p;
        p = p->next;
        free(temp);
     }
}
free(g->adjlist);
free(g);
}

void graph_insert_link(struct graph *g, int head_node, int tail_node, int weight)
{
struct adjlist_node * new_node = (struct adjlist_node *) malloc(sizeof(struct adjlist_node));
new_node->weight = weight;
new_node->node_id = tail_node;
new_node->next = g->adjlist[head_node];
g->adjlist[head_node] = new_node;
}

void graph_display(struct graph * g)
{
printf("Graph: -> node_id (weight)\n");
for (int i=0; i<g->num_nodes; i++) {
     printf("Node %2d: ",i);
     for (struct adjlist_node * p = g->adjlist[i]; p != NULL; p = p->next) {
           printf("->%2d (%2d) ", p->node_id, p->weight);
     }
     printf("\n");
}
}


struct algm_data * dijkstra(struct graph * g, int source)
{
struct algm_data * a = algm_data_create(g, source);
struct priorityq * pq = priorityq_create(a);

while (priorityq_not_empty(pq)) {
    int new_node = priorityq_get(pq, a);
    for (struct adjlist_node * p = g->adjlist[new_node]; p!=NULL; p=p->next) {
        if (priorityq_in_q(pq,p->node_id)==1) {
           if (a->node[p->node_id].dist > a->node[new_node].dist + p->weight) {
	      a->node[p->node_id].dist = a->node[new_node].dist + p->weight;
	      a->node[p->node_id].pred = new_node;
              priorityq_update(pq, a, p->node_id);
           }
        }
    }
}

priorityq_destroy(pq);
return a;
}

void algm_data_display(struct algm_data * a)
{
for (int i=0; i<a->num_nodes; i++) {
     printf("Node %2d: dist = %2d, pred = %2d\n", i, a->node[i].dist, a->node[i].pred);
}	
}

struct graph * create_graph0()
{
struct graph * g = graph_create(6);
graph_insert_link(g, 0, 1, 4);
graph_insert_link(g, 0, 2, 1);
graph_insert_link(g, 1, 3, 2);
graph_insert_link(g, 1, 4, 2);
graph_insert_link(g, 2, 1, 1);
graph_insert_link(g, 2, 3, 5);
graph_insert_link(g, 2, 4, 4);
graph_insert_link(g, 3, 5, 4);
graph_insert_link(g, 4, 2, 1);
graph_insert_link(g, 4, 5, 1);
return g;
}

struct graph * create_graph1()
{
struct graph * g = graph_create(10);
graph_insert_link(g, 0, 1, 2);
graph_insert_link(g, 0, 2, 1);
graph_insert_link(g, 1, 3, 2);
graph_insert_link(g, 2, 5, 10);
graph_insert_link(g, 2, 4, 20);
graph_insert_link(g, 3, 5, 1);
graph_insert_link(g, 3, 6, 10);
graph_insert_link(g, 4, 2, 1);
graph_insert_link(g, 4, 6, 1);
graph_insert_link(g, 4, 9, 1);
graph_insert_link(g, 5, 7, 1);
graph_insert_link(g, 6, 4, 1);
graph_insert_link(g, 6, 8, 1);
graph_insert_link(g, 7, 8, 1);
graph_insert_link(g, 8, 6, 1);
graph_insert_link(g, 8, 7, 1);
return g;
}


