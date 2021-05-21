#include<time.h>
#include<stdlib.h>
#include<stdio.h>

#define TPB 1

void TopDownMergeSort(int *A[], int n, int *B[]);
void TopDownSplitMerge(int *A[], int iBegin, int iEnd, int *B[]);
void TopDownMerge(int *A[], int iBegin, int iMiddle, int iEnd, int *B[]);
void CopyArray(int *B[], int iBegin, int iEnd, int *A[]);

__global__ void kruskal( int *a, double *h, double *area, double *y, double *n){

		int i = threadIdx.y + blockIdx.y * blockDim.y;
		int j = threadIdx.x + blockIdx.x * blockDim.x;

		if( i < *grid && j < *grid){
			y[index] = (*a) + index * (*h);
			area[index] = 1 + y[index] + sin(2*y[index]);
		}
}

int main( int argc, int **argv){

	clock_t begin, end;
	double time_spent;

	begin = clock();

	int tree[grid][grid], mst[grid][grid], nodeSet[grid], r, count = 0;
	srand(time(NULL));

	for(int i = 0; i < grid; i++){
		for(int j = 0; j < grid; j++){
			tree[i][j] = 0;
			mst[i][j] = 0;
		}
	}

	for(int i = 0; i < grid; i++){
		for(int j = 0; j < grid; j++){
			r = rand() % 100;
			if(!tree[i][j] && i < j && r){
				tree[j][i] = r;
				tree[i][j] = r;
				count++;
			}
		}
	}

	int **minedge = malloc(sizeof(int*)*count);
	int **tempedge = malloc(sizeof(int*)*count);

	for( int i = 0; i < count; i++){
		minedge[i] = malloc(2);
		tempedge[i] = malloc(2);
	}

	count = 0;

	for(int i = 0; i < grid; i++){
		for(int j = 0; j < grid; j++){
			if(tree[i][j] > 0 && i < j){
				minedge[count][0] = tree[i][j];
				minedge[count][1] = (i*grid)+j;
				count++;
			}
		}
	}

	for(int i = 0; i < grid; i++)
		nodeSet[i] = i;

	TopDownMergeSort( minedge, count, tempedge);

	int nodeA, nodeB;

	for(int i = 0; i < count; i++){
		nodeA = minedge[i][1]%grid;
		nodeB = minedge[i][1]/grid;
		if(nodeSet[nodeA] != nodeSet[nodeB]){
			for(int j = 0; j < grid; j++){
				if(nodeSet[j] == nodeSet[nodeB] && j != nodeB)
					nodeSet[j] = nodeSet[nodeA];
			}
			nodeSet[nodeB] = nodeSet[nodeA];
			mst[nodeA][nodeB] = minedge[i][0];
			mst[nodeB][nodeA] = minedge[i][0];
		}
	}

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	for(int i = -1; i < grid; i++){
		if( i >= 0)
			printf("\n%d ", i);
		else
			printf("\n  ");
		for(int j = 0; j < grid; j++){
			if(i == -1)
				printf("%d ", j);
			else
				printf("%c%c ", j <= i ? ' ' : tree[i][j]/10 > 0 ? tree[i][j]/10 + '0' : '\0', j <= i ? '\0' : tree[i][j]%10 + '0');
		}
		printf("\n");
	}

	for(int i = -1; i < grid; i++){
		if( i >= 0)
			printf("\n%d ", i);
		else
			printf("\n  ");
		for(int j = 0; j < grid; j++){
			if(i == -1)
				printf("%d ", j);
			else
				printf("%c%c ", j <= i ? ' ' : mst[i][j]/10 > 0 ? mst[i][j]/10 + '0' : '\0', j <= i ? '\0' : mst[i][j]%10 + '0');
		}
		printf("\n");
	}

	printf("timespent = %lf\n", time_spent);
}

// Array A[] has the items to sort; array B[] is a work array.
void TopDownMergeSort(int *A[], int n, int *B[])
{
    TopDownSplitMerge(A, 0, n, B);
}

// iBegin is inclusive; iEnd is exclusive (A[iEnd] is not in the set).
void TopDownSplitMerge(int *A[], int iBegin, int iEnd, int *B[])
{
    if(iEnd - iBegin < 2)                       // if run size == 1
        return;                                 //   consider it sorted
    // recursively split runs into two halves until run size == 1,
    // then merge them and return back up the call chain
    int iMiddle = (iEnd + iBegin) / 2;              // iMiddle = mid point
    TopDownSplitMerge(A, iBegin,  iMiddle, B);  // split / merge left  half
    TopDownSplitMerge(A, iMiddle,    iEnd, B);  // split / merge right half
    TopDownMerge(A, iBegin, iMiddle, iEnd, B);  // merge the two half runs
    CopyArray(B, iBegin, iEnd, A);              // copy the merged runs back to A
}

//  Left half is A[iBegin :iMiddle-1].
// Right half is A[iMiddle:iEnd-1   ].
void TopDownMerge(int *A[], int iBegin, int iMiddle, int iEnd, int *B[])
{
    int i = iBegin, j = iMiddle;
    
    // While there are elements in the left or right runs...
    for (int k = iBegin; k < iEnd; k++) {
        // If left run head exists and is <= existing right run head.
        if (i < iMiddle && (j >= iEnd || A[i][0] <= A[j][0])) {
            B[k][0] = A[i][0];
			B[k][1] = A[i][1];
            i = i + 1;
        } else {
            B[k][0] = A[j][0];
            B[k][1] = A[j][1];
            j = j + 1;    
        }
    } 
}

void CopyArray(int *B[], int iBegin, int iEnd, int *A[])
{
    for(int k = iBegin; k < iEnd; k++){
		A[k][0] = B[k][0];
		A[k][1] = B[k][1];
	}
}

