#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#include <time.h>
#include <stack>
#include "DBL.h"

//#define NO_PATH_OUT   // comment out this line for path output
void Error_Exit(const char *s);

typedef struct _Vertex {
	dblStack S;		// adj list contains edge index
	int degree;
} Vertex;

typedef struct _Edge {
	int v1, v2;
} Edge;

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN);
void adjListGenerate(Vertex *V, Edge *E, int VN, int EN);
void deallocGraph(Vertex *V, Edge *E, int VN);
int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN);

DBList pool;	// DBL storage pool

int main() {
	int T, VN, EN;
	Vertex *V;
	Edge   *E;
	int *path;	// Euler cycle or path
	int pathN;  // path length
	int  flag;	// 0: cycle, 1: path, 2: none
	clock_t start_time, finish_time;

	scanf("%d", &T);	// read # of tests
	for (int t = 1; t <= T; t++) {	// for each test
		graphGeneration(&V, &E, &VN, &EN);

		start_time = clock(); // set the start time

		path = Find_Euler(V, E, VN, EN, &flag, &pathN); // find an Euler path or cycle

		finish_time = clock(); // set finish time
		
		double cmpt = (((double)(finish_time - start_time)) / CLK_TCK)*1000; // compute the time passed
		printf("Test= %d flag= %d VnumInCycle/Path)= %d ", t, flag, pathN);

		if (flag == 0)
			printf("Euler_cycle(exec_time= %.2f msec)\n",cmpt);
		else if (flag == 1)
			printf("Euler_path(exec_time= %.2f msec)\n", cmpt);
		else
			printf("not_Eulerian(exec_time= %.2f msec)\n", cmpt);

#ifndef NO_PATH_OUT
		if (flag != 2) {
			for (int i = 0; i < EN + 1; i++) {
				printf("%d\n", path[i]);
			}
		}
#endif
		if (flag != 2)
			delete[] path;
		deallocGraph(V, E, VN);
	}
	pool.freeDBL_pool();	// clear all the DBL elements

	return 0;
}

int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN) {
	// input V, VN, E, EN
	// output through paramters
	//   *flag = 0 (Euler cycle), 1 (Euler path), 2 (not Eulerian)
	//   *pathN = size of path[] array
	// output by return
	//   *path = list of vertex ids found(Euler cycle or path)

	stack<int> ST;		// use STL stack as explained in the class
	int *path = NULL;
	path = new int[EN*10];
	int idx = 0;
	int count = 0;
	int v; //while문에서 사용할 변수
	int temp;
	DBL* p;
	DBL* tp;

	// *** 이 부분을 작성하세요
	for (int i = 0; i < VN; i++)
		if (V[i].degree % 2 == 1)
			count++;
	if (count == 0 || count == 2) //euler cycle
	{
		if (count == 0) {
			*flag = 0;
			ST.push(0);
		}
		else {
			*flag = 1;
			int k;
			for (k = 0; k < VN; k++)
				if (V[k].degree % 2 == 1)
					break;
			ST.push(k);
		}

		while (ST.empty() != true)
		{
			v = ST.top();
			if (V[v].degree == 0)
			{
				path[idx] = v;
				ST.pop();
				idx++;
			}
			else
			{
				temp = V[v].S.top()->d;
				if (E[temp].v1 == v)
				{
					ST.push(E[temp].v2);
					tp = V[v].S.top()->twin;
					p = V[v].S.pop();
					V[v].degree--;
					pool.freeDBL(p);
					V[E[temp].v2].S.remove(tp);
					V[E[temp].v2].degree--;
					pool.freeDBL(tp);
				}
				else
				{
					ST.push(E[temp].v1);
					tp = V[v].S.top()->twin;
					p = V[v].S.pop();
					V[v].degree--;
					pool.freeDBL(p);
					V[E[temp].v1].S.remove(tp);
					V[E[temp].v1].degree--;
					pool.freeDBL(tp);
				}
			}
		}
		
	}
	else {
		*flag = 2;
		for (int i = 0; i < VN; i++)
		{
			while (V[i].S.empty() == 0)
			{
				p = V[i].S.pop();
				pool.freeDBL(p);
			}
		}
	}

	*pathN = idx;
	return path;
}

void deallocGraph(Vertex *V, Edge *E, int VN) {
	DBL *p;

	// *** 여기에 adj list를 삭제하는 routine을 작성하세요
	
	for (int i = 0; i < VN; i++) {
		while (V->S.empty() == false) { //****
			p = V->S.pop();
			pool.freeDBL(p);
		}
	}

	delete[] V;
	delete[] E;
}

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN) {
	scanf("%d %d", VN, EN);	// read # of vertices and edges
	//allocVandEarray(V, E, *VN, *EN);	// vertex and edge array allocation

	*V = new Vertex[*VN];
	*E = new Edge[*EN];
	if (*V == NULL || *E == NULL) {
		Error_Exit("Memory Allocation Error!");
	}

	for (int v = 0; v < *VN; v++) {
		(*V)[v].degree = 0;
	}
	for (int e = 0; e < *EN; e++) {
		scanf("%d %d", &((*E)[e].v1), &((*E)[e].v2));	// read edge information
		++((*V)[(*E)[e].v1].degree); // ++ V[E[i].v1].degree
		++((*V)[(*E)[e].v2].degree);
	}
	adjListGenerate(*V, *E, *VN, *EN);	// create adj lust in G=(V,E)
}

void adjListGenerate(Vertex *V, Edge *E, int VN, int EN) {
	// Construct adjacent list in V array
	int v1, v2;
	DBL *adjE1, *adjE2;

	// *** 이 부분을 작성하세요
	for (int i = 0; i < EN; i++)
	{
		adjE1 = pool.allocDBL();
		adjE2 = pool.allocDBL();

		adjE1->lp = NULL;
		adjE1->rp = NULL;
		adjE1->d = i;
		adjE1->twin = adjE2;
		
		adjE2->lp = NULL;
		adjE2->rp = NULL;
		adjE2->d = i;
		adjE2->twin = adjE1;

		v1 = E[i].v1;
		v2 = E[i].v2;

		if (V[v1].S.empty() == true) {
			V[v1].S.push(adjE1);
		}
		else {
			adjE1->rp = V[v1].S.top();
			V[v1].S.top()->lp = adjE1;
			V[v1].S.push(adjE1);
		}

		if (V[v2].S.empty() == true) {
			V[v2].S.push(adjE2);
		}
		else {
			adjE2->rp = V[v2].S.top();
			V[v2].S.top()->lp = adjE2;
			V[v2].S.push(adjE2);
		}
	}
}

void Error_Exit(const char *s) {
	printf("%s", s);
	exit(-1);
}

DBL *DBList::allocDBL(void) {		// allocate a DBL element
	DBL *p;
	if (DBL_pool == NULL) {
		p = new DBL;
		if (p == NULL) {
			Error_Exit("Memory alloc error(Alloc_DBL)");
		}
		UsedMemoryForDBLs += sizeof(DBL);
		p->d = NONE;
	}
	else {
		p = DBL_pool;
		DBL_pool = p->rp;
	}
	p->rp = p->lp = p->twin = NULL;

	// *** 이 부분을 작성하세요
	++DBL_cnt;
	return(p);
}

void DBList::freeDBL(DBL *p) {
	// move p to pool

	if (p->d == NONE) {
		Error_Exit("This element is already freed(Free_DBL).");
	}
	
	// *** 이 부분을 작성하세요
	p->d = NONE;
	p->rp = DBL_pool;
	DBL_pool = p;
	--DBL_cnt;		// reduce # of active DBL elements by one 
}

void DBList::freeDBL_pool(void) {
	DBL *p = DBL_pool;

	while (p != NULL) {
		DBL_pool = p->rp;	// get next pointer
		delete p;
		p = DBL_pool;
		UsedMemoryForDBLs -= sizeof(DBL);
	}
	if (DBL_cnt != 0) {
		Error_Exit("Non-zero DBL_cnt after cleanup.");
	}
}

void dblStack::push(DBL *p) {

	// *** 이 부분을 작성하세요
	if (ST != NULL) {
		ST->lp = p;
	}
	p->rp = ST;
	p->lp = NULL;
	ST = p;
}

DBL *dblStack::pop(void) {	// remove and return p in front of Q
	DBL *p;
	p = ST;
	// *** 이 부분을 작성하세요
	if (ST->rp == NULL)
		ST = NULL;
	else {
		ST = ST->rp;
		ST->lp = NULL;
	}
	return p;
}

void dblStack::remove(DBL *p) {	// delete p from ST

	// *** 이 부분을 작성하세요
	if (ST == p) {
		ST = p->rp;
		if (p->rp != NULL)
			(p->rp)->lp = NULL;
	}
	else {
		(p->lp)->rp = p->rp;
		if (p->rp != NULL)
			(p->rp)->lp = p->lp;
	}
}

DBL *dblStack::top(void) {
	return ST;
}

bool dblStack::empty(void) {
	if (ST == NULL)
		return true;
	else
		return false;
}
