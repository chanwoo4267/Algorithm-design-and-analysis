#include <algorithm>
#include <stdio.h>

void Edit_Distance(
	// inputs
	char* SS,		// Source string array
	char* TS,		// Edited string array
	int ins_cost, int del_cost, int sub_cost,
	// insertion, deletion, substitution cost
// outputs
int** Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
char** SR,		// Source string with insertion to T marks '*'
char** OP,		// Operation sequence('i', 'd', 's', or '.' (nop))
char** TR,       // Edited string with deletion form S mark '*'
				// NOTE: ST, OP, TR must be allocated inside the function as needed(minimum as possible)
	int* Mem_Allocated
)
{
	int sslen = strlen(SS);
	int tslen = strlen(TS);

	for (int i = 0; i <= sslen; i++)
		Table[i][0] = i * del_cost;
	for (int i = 0; i <= tslen; i++)
		Table[0][i] = i * ins_cost;                     //0번 열, 0번행의 초기화

	for (int i = 1; i <= sslen; i++)
		for (int j = 1; j <= tslen; j++)
		{
			if (SS[i - 1] == TS[j - 1]) {
				Table[i][j] = std::min({ Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + 0 });
			}
			else
				Table[i][j] = std::min({ Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + sub_cost });
		}												//ins, del, sub중 min을 찾아서 배열 채우기

	int len = std::max({ Table[sslen][tslen] + 1 , sslen, tslen });

	*SR = (char*)malloc(sizeof(char) * len + 1);
	*OP = (char*)malloc(sizeof(char) * len + 1);
	*TR = (char*)malloc(sizeof(char) * len + 1);
	*Mem_Allocated = sizeof(char) * len * 3 + 3;

	(*SR)[len] = NULL;
	(*OP)[len] = NULL;
	(*TR)[len] = NULL;

	int a = sslen, b = tslen;								//a,b : ss의 idx, ts의 idx
	int t;
	int idx = len - 1;
	int idxs = sslen - 1;								//idx = 문자열 idx
	int idxt = tslen - 1;								//idxt = ts에 *을 적절히 삽입할 문자열의 idx
														//idxs = ss에 *을 적절히 삽입할 문자열의 idx
	while (!(a == 0 && b == 0))								//(0,0)에 도달하면 종료
	{
		if (SS[a - 1] == TS[b - 1])						//SS, TS 각각의 idx가 같은 문자라면
		{
			if (a <= 0)
				t = Table[a][b - 1] + ins_cost;
			else if (b <= 0)
				t = Table[a - 1][b] + del_cost;
			else
				t = std::min({ Table[a][b - 1] + ins_cost, Table[a - 1][b] + del_cost, Table[a - 1][b - 1] + 0 }); //a or b idx가 0일때 예외처리

			if (ins_cost <= del_cost) {						//parent 선택이 둘 이상 존재할경우 ins <= del이면 ins 우선선택
				if (b != 0 && t == Table[a][b - 1] + ins_cost)	//insert가 min이면
				{
					(*OP)[idx] = 'i';
					(*SR)[idx] = '*';
					(*TR)[idx--] = TS[idxt--];
					b--;
				}
				else if (a != 0 && t == Table[a - 1][b] + del_cost)	//delete가 min이면
				{
					(*OP)[idx] = 'd';
					(*SR)[idx] = SS[idxs--];
					(*TR)[idx--] = '*';
					a--;
				}
				else											//두 문자가 같은 문자이므로 ins, del, sub 수행안함
				{
					(*OP)[idx] = '.';
					(*SR)[idx] = SS[idxs--];
					(*TR)[idx--] = TS[idxt--];
					a--; b--;
				}
			}

			else {
				if (a != 0 && t == Table[a - 1][b] + del_cost)
				{
					(*OP)[idx] = 'd';
					(*SR)[idx] = SS[idxs--];
					(*TR)[idx--] = '*';
					a--;
				}
				else if (b != 0 && t == Table[a][b - 1] + ins_cost)
				{
					(*OP)[idx] = 'i';
					(*SR)[idx] = '*';
					(*TR)[idx--] = TS[idxt--];
					b--;
				}
				else
				{
					(*OP)[idx] = '.';
					(*SR)[idx] = SS[idxs--];
					(*TR)[idx--] = TS[idxt--];
					a--; b--;
				}
			}
		}

		else
		{
			if (a <= 0)
				t = Table[a][b - 1] + ins_cost;
			else if (b <= 0)
				t = Table[a - 1][b] + del_cost;
			else
				t = std::min({ Table[a][b - 1] + ins_cost, Table[a - 1][b] + del_cost, Table[a - 1][b - 1] + sub_cost }); //예외처리

			if (ins_cost <= del_cost) {		//ins 우선선택
				if (b != 0 && t == Table[a][b - 1] + ins_cost)
				{
					(*OP)[idx] = 'i';
					(*SR)[idx] = '*';
					(*TR)[idx--] = TS[idxt--];
					b--;
				}
				else if (a != 0 && t == Table[a - 1][b] + del_cost)
				{
					(*OP)[idx] = 'd';
					(*SR)[idx] = SS[idxs--];
					(*TR)[idx--] = '*';
					a--;
				}
				else						//ss와 ts 문자가 같지 않으므로 substitution 한다
				{
					(*OP)[idx] = 's';
					(*SR)[idx] = SS[idxs--];
					(*TR)[idx--] = TS[idxt--];
					a--; b--;
				}
			}

			else {
				if (a != 0 && t == Table[a - 1][b] + del_cost)
				{
					(*OP)[idx] = 'd';
					(*SR)[idx] = SS[idxs--];
					(*TR)[idx--] = '*';
					a--;
				}
				else if (b != 0 && t == Table[a][b - 1] + ins_cost)
				{
					(*OP)[idx] = 'i';
					(*SR)[idx] = '*';
					(*TR)[idx--] = TS[idxt--];
					b--;
				}
				else
				{
					(*OP)[idx] = 's';
					(*SR)[idx] = SS[idxs--];
					(*TR)[idx--] = TS[idxt--];
					a--; b--;
				}
			}
		}
	}
}