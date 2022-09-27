#include "Graph_adj_array.h"

void Read_Graph_adj_array (int Vnum, vertex V[], int Enum, edge E[]) {
	// read graph information
	// V와 E의 name field는 각각 자신의 인덱스를 저장한다
	// V[].flag과 E[].flag은 반드시 false로 설정해야 한다.
	
	// ***이 함수를 작성한다
	int vf, vr, cost;
	for (int i = 0; i < Vnum; i++)
	{
		V[i].flag = false;
		V[i].name = i;
		V[i].f_hd = -1;
		V[i].r_hd = -1;
	}

	for (int i = 0; i < Enum; i++)
	{
		E[i].fp = -1;
		E[i].rp = -1;
	}

	for (int i = 0; i < Enum; i++)
	{
		scanf_s("%d %d %d", &vf, &vr, &cost);
		E[i].vf = vf;
		E[i].vr = vr;
		E[i].cost = cost;
		E[i].name = i;

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

void dfs(int cur, int Vnum, vertex* V, int Enum, edge* E)
{
	V[cur].flag = true;
	int n = V[cur].f_hd;
	while (1)
	{
		if (n == -1) break;
		if (V[E[n].vf].flag == false)
		{
			E[n].flag = true;
			dfs(E[n].vf, Vnum, V, Enum, E);
		}
		if (V[E[n].vr].flag == false)
		{
			E[n].flag = true;
			dfs(E[n].vr, Vnum, V, Enum, E);
		}
		n = E[n].fp;
	}
	n = V[cur].r_hd;
	while (1)
	{
		if (n == -1) break;
		if (V[E[n].vf].flag == false)
		{
			E[n].flag = true;
			dfs(E[n].vf, Vnum, V, Enum, E);
		}
		if (V[E[n].vr].flag == false)
		{
			E[n].flag = true;
			dfs(E[n].vr, Vnum, V, Enum, E);
		}
		n = E[n].rp;
	}
}

int DFS_Tree_adj_array(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0) 
) {  
	// DFS를 사용하여 DFS tree에 속하는 에지의 flag을 true로 설정한다.
	// DFS 시작 vertex는 입력 파일에서 지정된다(src).
	// DFS tree에 속한 에지의 cost 함을 return 한다(모든 각 edge cost가 1이면 unique)
	// recursive 함수로 작성해도 무방하다.

	// ***이 함수를 작성한다***
	int cost=0;
	dfs(src, Vnum, V, Enum, E);
	for (int i = 0; i < Enum; i++)
		if (E[i].flag == 1) cost += E[i].cost;
	return cost;
}

