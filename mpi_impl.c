#include<time.h>
#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>

void TopDownMergeSort(int *A[], int n, int *B[]);
void TopDownSplitMerge(int *A[], int iBegin, int iEnd, int *B[]);
void TopDownMerge(int *A[], int iBegin, int iMiddle, int iEnd, int *B[]);
void CopyArray(int *B[], int iBegin, int iEnd, int *A[]);
int **alloc_2d_int(int rows, int cols);

int main( int argc, char **argv){

	clock_t begin, end;
	double time_spent;
	int grid = atoi(argv[1]);

	begin = clock();

	int tree[grid][grid], mst[grid][grid], nodeSet[grid], r, count = 0, id, size, remainder;
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

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Status stat;
	remainder = count%size;
	if(id+1 == size)
		count = count/size+remainder;
	else
		count = count/size;

	int final_count = count;

//	printf("count = %d, id = %d\n", count, id);

	int **minedge;
	int **tempedge;

	minedge = alloc_2d_int(count, 2);
	tempedge = alloc_2d_int(count, 2);

	count = 0;
	int tempcount = 0;

	for(int i = 0; i < grid; i++){
		for(int j = 0; j < i; j++){
			if(tree[i][j] > 0){
//				printf("i = %d j = %d tree[i][j] = %d id = %d tempcount = %d\n", i, j, tree[i][j], id, tempcount);
				if((tempcount == id || (id+1 == size && count+remainder >= final_count)) && count < final_count){
					minedge[count][0] = tree[i][j];
					minedge[count][1] = (i*grid)+j;
//					printf("minedge[%d][0] = %d, minedge[%d][1] = %d, id = %d\n", count, minedge[count][0], count, minedge[count][1], id);
					count++;
				}
				if(++tempcount == size)
					tempcount = 0;
			}
		}
	}

	count = final_count;

	for(int i = 0; i < grid; i++)
		nodeSet[i] = i;

	TopDownMergeSort( minedge, count, tempedge);

	int nodeA, nodeB;

	for(int i = 0; i < count; i++){
		nodeA = minedge[i][1]%grid;
		nodeB = minedge[i][1]/grid;
//		printf("minedge[%d][0] = %d, minedge[%d][1] = %d, id = %d\n", i, minedge[i][0], i, minedge[i][1], id);
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

	int send = 1;

	while(1 && size > 1){

		if(id%(send*2) == send && id != 0){
			MPI_Send(&count,1,MPI_INT,id-send,send*2,MPI_COMM_WORLD);
			MPI_Send(&(minedge[0][0]),2*count,MPI_INT,id-send,0,MPI_COMM_WORLD);
			break;
		}

		int recv_count, source;
		MPI_Recv(&recv_count, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
		int **recv_edge;
		recv_edge = alloc_2d_int(recv_count, 2);
		send = stat.MPI_TAG;
		source = stat.MPI_SOURCE;

		MPI_Recv(&(recv_edge[0][0]), recv_count*2, MPI_INT, source, 0, MPI_COMM_WORLD, &stat);

		for(int i = 0; i < recv_count; i++){
			nodeA = recv_edge[i][1]%grid;
			nodeB = recv_edge[i][1]/grid;
			if(mst[nodeA][nodeB] > recv_edge[i][0] || !mst[nodeA][nodeB]){
				mst[nodeA][nodeB] = recv_edge[i][0];
				mst[nodeB][nodeA] = recv_edge[i][0];
			}
		}

		count = count + recv_count;

		minedge = alloc_2d_int(count, 2);
		tempedge = alloc_2d_int(count, 2);

		count = 0;

		for(int i = 0; i < grid; i++){
			for(int j = 0; j < i; j++){
				if(mst[i][j] > 0){
					minedge[count][0] = mst[i][j];
					minedge[count][1] = (i*grid)+j;
					count++;
				}
			}
		}

		for(int i = 0; i < grid; i++)
			nodeSet[i] = i;

		TopDownMergeSort( minedge, count, tempedge);

		for(int i = 0; i < grid; i++){
			for(int j = 0; j < grid; j++)
				mst[i][j] = 0;
		}

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

		if( send == size && id == 0)
			break;
	}

	if(id == 0){
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		if (grid < 15){
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
		}
		printf("timespent = %lf\n", time_spent);
	}
	MPI_Finalize();
	return 0;
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

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}

