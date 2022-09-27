#include <algorithm>
using namespace std;

void merge(double* X, unsigned* Xid, unsigned* TMP, unsigned N, int left, int mid, int right)
{
	int i, j, k, l;
	i = left, j = mid + 1, k = left;

	while (i <= mid && j <= right)
	{
		if (X[Xid[i]] <= X[Xid[j]])
			TMP[k++] = Xid[i++];
		else
			TMP[k++] = Xid[j++];
	}

	if (i > mid)
		for (l = j; l <= right; l++)
			TMP[k++] = Xid[l];
	else
		for (l = i; l <= mid; l++)
			TMP[k++] = Xid[l];

	for (l = left; l <= right; l++)
	{
		Xid[l] = TMP[l];
	}
}

void merge_sort(double* X, unsigned* Xid, unsigned* TMP, unsigned N, int left, int right)
{
	int mid;
	if (left < right)
	{
		mid = (left + right) / 2;
		merge_sort(X, Xid, TMP, N, left, mid);
		merge_sort(X, Xid, TMP, N, mid + 1, right);
		merge(X, Xid, TMP, N, left, mid, right);
	}
}

void sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N)
{
	merge_sort(X, Xid, TMP, N, 0, N - 1);
}

// input  : X[]   (x position array of N points)
//          Xid[] (index array of X (initially [0,1,...,N-1])
//          TMP[] (temporary array of size N. may or may not be used)
//          N   number of points to sort
// output : Xid[] (sorted indices by x position in X[]){{

void sortYid(unsigned left, unsigned right, double* X, double* Y, unsigned* Xid, unsigned* Yid)
{
	for (unsigned i = left; i < right + 1; i++)
		Yid[i] = Xid[i];
	for (unsigned i = left; i < right + 1; i++)
		for (unsigned j = i + 1; j < right + 1; j++)
			if (Y[Yid[i]] > Y[Yid[j]]) swap(Yid[i], Yid[j]);
}
double closestPairDC_r(unsigned L, unsigned R,	  // current leftmost and rightmost indices
	unsigned* pt1, unsigned* pt2, // closest pair points indices
	double* X, double* Y,         // (x,y) positions array(input)
	unsigned* Xid,  // point index array(sorted by x coordinates)
	unsigned* Yid,  // point index array(gradually sorted by y coordinates)
	unsigned* TMP,  // temporal index array
	unsigned THR	// threshold value
)
{
	unsigned int a, b;
	double dist = sqrt((X[*pt1] - X[*pt2]) * (X[*pt1] - X[*pt2]) + (Y[*pt1] - Y[*pt2]) * (Y[*pt1] - Y[*pt2]));
	if (L > R) return dist;

	if (R - L + 1 <= THR)
	{
		sortYid(L, R, X, Y, Xid, Yid);
		for (unsigned i = L; i < R; i++)
			for (unsigned j = i + 1; j <= R; j++)
				if (dist > sqrt((Y[i] - Y[j]) * (Y[i] - Y[j]) + (X[i] - X[j]) * (X[i] - X[j])))
				{
					dist = sqrt((Y[i] - Y[j]) * (Y[i] - Y[j]) + (X[i] - X[j]) * (X[i] - X[j]));
					a = i, b = j;
					*pt1 = a;
					*pt2 = b;
				}
	}
	else
	{
		unsigned mid;
		if (L + R % 2 == 1) mid = (L + R - 1) / 2;
		else mid = (L + R) / 2;

		dist = min(dist, closestPairDC_r(L, mid, pt1, pt2, X, Y, Xid, Yid, TMP, THR));
		dist = min(dist, closestPairDC_r(mid + 1, R, pt1, pt2, X, Y, Xid, Yid, TMP, THR));

		//sort Yid
		unsigned i, j, k, l;
		i = L, j = mid + 1, k = L;

		while (i <= mid && j <= R)
		{
			if (Y[Yid[i]] < Y[Yid[j]])
				TMP[k++] = Yid[i++];
			else
				TMP[k++] = Yid[j++];
		}

		if (i > mid)
			for (l = j; l <= R; l++)
				TMP[k++] = Yid[l];
		else
			for (l = i; l <= mid; l++)
				TMP[k++] = Yid[l];

		for (l = L; l <= R; l++)
		{
			Yid[l] = TMP[l];
		}

		double Xmid = (X[Xid[mid]] + X[Xid[mid + 1]]) / 2;
		unsigned idx = 0;
		// Xmid로부터 dist보다 가까운 점들의 Yid만 TMP에 삽입

		for (unsigned i = L; i <= R; i++)
		{
			if (Xmid - dist <= X[Yid[i]] && X[Yid[i]] <= Xmid + dist)
			{
				TMP[idx] = Yid[i];
				idx++;
			}
		}

		for (unsigned i = 0; i < idx; i++)
		{
			for (unsigned j = i + 1; j < idx; j++)
			{
				if (dist > sqrt((X[TMP[i]] - X[TMP[j]]) * (X[TMP[i]] - X[TMP[j]]) + (Y[TMP[i]] - Y[TMP[j]]) * (Y[TMP[i]] - Y[TMP[j]])))
				{
					dist = sqrt((X[TMP[i]] - X[TMP[j]]) * (X[TMP[i]] - X[TMP[j]]) + (Y[TMP[i]] - Y[TMP[j]]) * (Y[TMP[i]] - Y[TMP[j]]));
					a = TMP[i], b = TMP[j];
					*pt1 = a;
					*pt2 = b;
				}
			}
		}

	}
	return dist;
}

double closestPairDC(
	unsigned L, unsigned R,	  // current leftmost and rightmost indices
	unsigned* pt1, unsigned* pt2, // closest pair points indices
	double* X, double* Y,         // (x,y) positions array(input)
	unsigned* Xid,  // point index array(sorted by x coordinates)
	unsigned* Yid,  // point index array(gradually sorted by y coordinates)
	unsigned* TMP,  // temporal index array
	unsigned THR	// threshold value
)
{
	*pt1 = 0; *pt2 = 1;
	return closestPairDC_r(L, R, pt1, pt2, X, Y, Xid, Yid, TMP, THR);
}