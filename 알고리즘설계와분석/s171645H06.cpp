#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <algorithm>

#define NONE -1

typedef struct elm_edge {
	int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
	int  cost;    // edge cost
	bool flag;    // true if the edge is in the SP spanning tree, false otherwise 
	int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
	int  f_hd, r_hd;	// adj list header(-1 if none)
	int  distance;		// distance from src to this vertex
	int  heap_idx;		// minHeap index
	bool inS;			// true if SP length found(true when move from V-S to S) 
} vertex;

/*--------------------------------------------------
Heap Operation 1
when given index of Vertex, Insert into heap and adjust
when inserted, add heap_len 1
--------------------------------------------------*/
void heap_push(vertex *V, edge *E, int* minHeap, int idx, int* heap_len)
{
	minHeap[++*heap_len] = idx; // heap_len + 1 and add vertex index to heap
	int child = *heap_len; // from final value of heap, compare start
	int parent = child / 2;

	while (child > 1 && V[minHeap[parent]].distance > V[minHeap[child]].distance) //if Heap's parent's distance is larger than Heap's child's distance, Swap
	{
		std::swap(minHeap[parent], minHeap[child]);
		child = parent;                                  //move to parent index
		parent = child / 2;
	}
}
/*--------------------------------------------------
Heap Operation 2
delete heap element and adjust
delete means move first element to final index of heap, and sub heap_len 1
--------------------------------------------------*/
int heap_pop(vertex *V, edge *E, int* minHeap, int *heap_len)
{
	int result = minHeap[1]; //pop smallest
	
	std::swap(minHeap[1], minHeap[*heap_len]); //swap smallest element with final value of heap
	(*heap_len)--;

	int parent = 1; //start compare from first element
	int child = parent * 2;
	if (child + 1 <= *heap_len)
		child = (V[minHeap[child]].distance < V[minHeap[child + 1]].distance) ? child : child + 1; //select smaller child

	while (child <= *heap_len && V[minHeap[parent]].distance > V[minHeap[child]].distance) //if Heap's parent's distance is larger than child's distance, Swap
	{
		std::swap(minHeap[parent], minHeap[child]);
		parent = child;								//move to child index
		child = child * 2;
		if (child + 1 <= *heap_len)
			child = (V[minHeap[child]].distance < V[minHeap[child + 1]].distance) ? child : child + 1;
	}

	return result;
}
/*--------------------------------------------------
Heap Operation 3
when operation dijkstra, the V.distance may change when a new V move to S
because V.distance is changed, adjust Heap sequence
from Dijkstra function, if V[i].distance is changed, give parameter of its index i
heap_len is unchanged
--------------------------------------------------*/
void heap_move(vertex *V, edge *E, int* minHeap, int* heap_len, int idx)
{
	int child, parent;
	for(int i=1; i <= *heap_len; i++)
		if (minHeap[i] == idx)				//find V.distance_changed index
		{
			child = i;
			parent = i / 2;
		}

	while (child > 1 && V[minHeap[child]].distance < V[minHeap[parent]].distance) //if child reach root node, end
	{
		std::swap(minHeap[parent], minHeap[child]);
		child = parent;
		parent = parent / 2;
	}
}

int SPT_Dijkstra(
	int src,	// source vertex index
	// graph structure array
	// 1. the adjacency list structure is the same as PHW02
	// 2. additional fields are added for Dijkstra's algorithm(see .h file)
	int Vnum, vertex *V,	// Vertex array size and the array
	int Enum, edge *E,		// Edge array size and the array

	int *minHeap	// array for min heap (array size = Vnum+1)
		// heap index range is 1 ~ (Vnum - 1) note: src must not in the initial heap
		// just arry is passed. must construct min-heap in this function

	// OUTPUT
	// 1. V[].distance : shortest path length from src to this vertex
	// 2. shortest path spanning tree : set E[].flag = true if the edge is in the tree
	// 3. return the sum of edge costs in the shortest path spanning tree.
	//    should be small as possible (think a way to make it small)
) {
	int treeCost = 0;
	// *** 이 함수를 작성하자 ***
	// 반드시 min-heap을 사용하여 O((n+m)logn) 알고리즘을 구현해야 한다(아니면 trivial한 프로그램임)
	// heap 연산 등 필요한 함수는 자유롭게 작성하여 추가한다.
	// 그러나 global 변수, dynamic array 등은 추가로 사용하지 않는다(실제로 필요 없다)
	
	int heap_len = 0;

	V[src].distance = 0;
	V[src].inS = true;

	int u=src;							//current vertex index
	int t_v = -1;					//temporary vertex index

	for (int i = 0; i < Enum; i++) //Initialize Vertex distance from Source
	{
		if (E[i].vf == src) //left side is Source
			V[E[i].vr].distance = E[i].cost;

		else if (E[i].vr == src) //right side is Source
			V[E[i].vf].distance = E[i].cost;
	}

	//construct min_heap
	for (int i = 0; i < Vnum; i++)
	{
		if (i == src) continue;
		heap_push(V, E, minHeap, i, &heap_len);
	}

	bool SPT_flag;			//flag used when finding Spanning Tree

	for (int j = 0; j < Vnum - 1; j++)
	{
		SPT_flag = false;
		u = heap_pop(V, E, minHeap, &heap_len); //remove u from heap

		V[u].inS = true; //vertex flag set

		/* 
		Find Adjacent Vertex from U, and if it isn't in S and distance should be renewaled,
		renewal its distance and make heap calculation
		time complexity : 2mlogn ( because all vertex must be visited by twice )

		when we pop u from heap, it means V[u] must be in S
		this means, when we renew {V-S}.distance, find distance[v] = distance[u] - cost(u,v)
		edge (u,v) is in SPT
		it may be overlapped, so use flag SPT_flag
		if we get edge (u,v), turn its flag true and SPT_flag true
		*/
	
		if (V[u].f_hd != -1) //case 1
		{
			t_v = V[u].f_hd;
			while (1)			//for all adjacent vertex of V[u]
			{
				if (E[t_v].vf == u) //adjacent vertex = E[t_v].vr
				{
					//get SPT's edge
					if (V[E[t_v].vf].distance == V[E[t_v].vr].distance + E[t_v].cost && E[t_v].flag == false && SPT_flag == false) //if adjacent vertex's distance + between edge cost = this.distance, and flag is false
					{
						E[t_v].flag = true;
						treeCost += E[t_v].cost;
						SPT_flag = true;
					}
					//adjust adjacent vertex's distance
					if (V[E[t_v].vr].inS == false && V[u].distance + E[t_v].cost < V[E[t_v].vr].distance) //if adjacent vertex is unvisited and distance becomes smaller
					{
						V[E[t_v].vr].distance = V[u].distance + E[t_v].cost; //renew distance
						heap_move(V, E, minHeap, &heap_len, E[t_v].vr);	//adjust heap sequence
					}

					if (E[t_v].fp != -1)	//move to next adjacent vertex
						t_v = E[t_v].fp;
					else
						break;
				}

				else if (E[t_v].vr == u) //adjacent vertex = E[t_v].vf
				{
					if (V[E[t_v].vr].distance == V[E[t_v].vf].distance + E[t_v].cost && E[t_v].flag == false && SPT_flag == false) //if adjacent vertex's distance + between edge cost = this.distance, and flag is false
					{
						E[t_v].flag = true;
						treeCost += E[t_v].cost;
						SPT_flag = true;
					}
					
					if (V[E[t_v].vf].inS == false && V[u].distance + E[t_v].cost < V[E[t_v].vf].distance)
					{
						V[E[t_v].vf].distance = V[u].distance + E[t_v].cost;
						heap_move(V, E, minHeap, &heap_len, E[t_v].vf);
					}

					if (E[t_v].rp != -1)
						t_v = E[t_v].rp;
					else
						break;
				}
			}
		}

		if (V[u].r_hd != -1) //case 2
		{
			t_v = V[u].r_hd;
			while (1)
			{
				if (E[t_v].vf == u) //adjacent vertex = E[t_v].vr
				{
					if (V[E[t_v].vf].distance == V[E[t_v].vr].distance + E[t_v].cost && E[t_v].flag == false && SPT_flag == false) //if adjacent vertex's distance + between edge cost = this.distance, and flag is false
					{
						E[t_v].flag = true;
						treeCost += E[t_v].cost;
						SPT_flag = true;
					}
					
					if (V[E[t_v].vr].inS == false && V[u].distance + E[t_v].cost < V[E[t_v].vr].distance)
					{
						V[E[t_v].vr].distance = V[u].distance + E[t_v].cost;
						heap_move(V, E, minHeap, &heap_len, E[t_v].vr);
					}

					if (E[t_v].fp != -1)
						t_v = E[t_v].fp;
					else
						break;
				}

				else if (E[t_v].vr == u) //adjacent vertex = E[t_v].vf
				{
					if (V[E[t_v].vr].distance == V[E[t_v].vf].distance + E[t_v].cost && E[t_v].flag == false && SPT_flag == false) //if adjacent vertex's distance + between edge cost = this.distance, and flag is false
					{
						E[t_v].flag = true;
						treeCost += E[t_v].cost;
						SPT_flag = true;
					}
					
					if (V[E[t_v].vf].inS == false && V[u].distance + E[t_v].cost < V[E[t_v].vf].distance)
					{
						V[E[t_v].vf].distance = V[u].distance + E[t_v].cost;
						heap_move(V, E, minHeap, &heap_len, E[t_v].vf);
					}

					if (E[t_v].rp != -1)
						t_v = E[t_v].rp;
					else
						break;
				}
			}
		}
	}

	return treeCost;
}

void Read_Graph(int Vnum, vertex *V, int Enum, edge *E) {
	// Graph 자료구조를 만드는 함수
	// *** 이 함수를 추가하자 ***
	// PHW02의 Read_Graph_adj_array()를 이 과제의 자료구조에 맞춰 살짝 수정하여 사용한다
	// 즉, Read_Graph_adj_array()에서 불필요한 내용을 제거한 후 사용.

	int vf, vr, cost;
	for (int i = 0; i < Vnum; i++)	//initialize V
	{
		V[i].f_hd = -1;
		V[i].r_hd = -1;
		V[i].distance = 999999;
		V[i].inS = false;
	}

	for (int i = 0; i < Enum; i++)		//initialize E
	{
		E[i].fp = -1;
		E[i].rp = -1;
		E[i].flag = false;
	}

	for (int i = 0; i < Enum; i++)				//scan Edges and make data_structure
	{
		scanf_s("%d %d %d", &vf, &vr, &cost);
		E[i].vf = vf;
		E[i].vr = vr;
		E[i].cost = cost;

		if (V[vf].f_hd == -1)
			V[vf].f_hd = i;
		else
		{
			E[i].fp = V[vf].f_hd;
			V[vf].f_hd = i;
		}

		if (V[vr].r_hd == -1)
			V[vr].r_hd = i;
		else
		{
			E[i].rp = V[vr].r_hd;
			V[vr].r_hd = i;
		}
	}
}

// the following functions are for testing if the submitted program is correct.
int  Tree_Check(int Vnum, vertex *V, int Enum, edge *E, int *visited);
bool SPT_test(int src, int Vnum, vertex *V, int Enum, edge *E, int *minHeap);
void Error_Exit(const char *s);

int main ( void ) {
	int		src;
	vertex *V;		int Vnum;
	edge   *E;		int Enum;
	int    *minHeap;
	int    Tree_cost;
	int    Tnum; 		// # of test cases
	clock_t start, finish;
	double cmpt;

	scanf_s("%d", &Tnum);		// read # of tests

	for (int t = 0; t < Tnum; t++ ) {
		scanf_s("%d %d %d", &Vnum, &Enum, &src);
		V = new vertex [Vnum];
		E = new edge [Enum];
		minHeap = new int[Vnum + 1];	// heap array allocation
		if ( V == NULL || E == NULL || minHeap == NULL ) {
			Error_Exit("Memory Allocation Error");
		}
		Read_Graph(Vnum, V, Enum, E);

		/**/start = clock();	// start timer

		Tree_cost = SPT_Dijkstra(src, Vnum, V, Enum, E, minHeap);	// code by students

		/**/finish = clock();	// stop timer
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;

		// 아래 Tree_Check와 SPT_test 함수는 첨부한 SPT_test.obj 파일에 있다.
		// 이 테스트에서 오류로 인하여 프로그램이 정지하면 뭔가 잘못된 것이다(제출해도 0점)
        if (Tree_Check(Vnum, V, Enum, E, minHeap) == 0) {
			Error_Exit("   ERROR The result is not a spanning tree");
		}
		if (SPT_test(src, Vnum, V, Enum, E, minHeap) == false) {
			Error_Exit("** Something wrong in applying Dijkstra's");
		}
		if ( t != 0 ) 
			printf("\n");

		printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n", 
			t+1, Vnum, Enum, cmpt, Tree_cost);

		delete [] minHeap; delete [] V; delete [] E;
	}
	return 0;
}

void Error_Exit (const char *s ) {
  printf("%s\n", s);
  exit(-1);
}
