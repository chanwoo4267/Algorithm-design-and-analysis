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
		Table[0][i] = i * ins_cost;                     //0�� ��, 0������ �ʱ�ȭ

	for (int i = 1; i <= sslen; i++)
		for (int j = 1; j <= tslen; j++)
		{
			if (SS[i - 1] == TS[j - 1]) {
				Table[i][j] = std::min({ Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + 0 });
			}
			else
				Table[i][j] = std::min({ Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + sub_cost });
		}												//ins, del, sub�� min�� ã�Ƽ� �迭 ä���

	int len = std::max({ Table[sslen][tslen] + 1 , sslen, tslen });

	*SR = (char*)malloc(sizeof(char) * len + 1);
	*OP = (char*)malloc(sizeof(char) * len + 1);
	*TR = (char*)malloc(sizeof(char) * len + 1);
	*Mem_Allocated = sizeof(char) * len * 3 + 3;

	(*SR)[len] = NULL;
	(*OP)[len] = NULL;
	(*TR)[len] = NULL;

	int a = sslen, b = tslen;								//a,b : ss�� idx, ts�� idx
	int t;
	int idx = len - 1;
	int idxs = sslen - 1;								//idx = ���ڿ� idx
	int idxt = tslen - 1;								//idxt = ts�� *�� ������ ������ ���ڿ��� idx
														//idxs = ss�� *�� ������ ������ ���ڿ��� idx
	while (!(a == 0 && b == 0))								//(0,0)�� �����ϸ� ����
	{
		if (SS[a - 1] == TS[b - 1])						//SS, TS ������ idx�� ���� ���ڶ��
		{
			if (a <= 0)
				t = Table[a][b - 1] + ins_cost;
			else if (b <= 0)
				t = Table[a - 1][b] + del_cost;
			else
				t = std::min({ Table[a][b - 1] + ins_cost, Table[a - 1][b] + del_cost, Table[a - 1][b - 1] + 0 }); //a or b idx�� 0�϶� ����ó��

			if (ins_cost <= del_cost) {						//parent ������ �� �̻� �����Ұ�� ins <= del�̸� ins �켱����
				if (b != 0 && t == Table[a][b - 1] + ins_cost)	//insert�� min�̸�
				{
					(*OP)[idx] = 'i';
					(*SR)[idx] = '*';
					(*TR)[idx--] = TS[idxt--];
					b--;
				}
				else if (a != 0 && t == Table[a - 1][b] + del_cost)	//delete�� min�̸�
				{
					(*OP)[idx] = 'd';
					(*SR)[idx] = SS[idxs--];
					(*TR)[idx--] = '*';
					a--;
				}
				else											//�� ���ڰ� ���� �����̹Ƿ� ins, del, sub �������
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
				t = std::min({ Table[a][b - 1] + ins_cost, Table[a - 1][b] + del_cost, Table[a - 1][b - 1] + sub_cost }); //����ó��

			if (ins_cost <= del_cost) {		//ins �켱����
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
				else						//ss�� ts ���ڰ� ���� �����Ƿ� substitution �Ѵ�
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