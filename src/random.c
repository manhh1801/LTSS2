#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../lib/mpi.h"
// #include <mpi.h>

/* Process information */
int ProcessID, Processes;
/*  */

/* Random integer */
void random(int* Array, int Size, int LowerBound, int UpperBound, int* Min, int* Max) {
  *Min = UpperBound, *Max = LowerBound;
  int Range = UpperBound - LowerBound + 1;
  for(int index = ProcessID; index < Size; index += Processes) {
    Array[index] = rand() % Range + LowerBound;
    if(Min != NULL && Array[index] < *Min) { *Min = Array[index]; }
    if(Max != NULL && Array[index] > *Max) { *Max = Array[index]; }
  }
}
/*  */

/* Entry procedure */
int main(int argc, char* argv[]) {
  /* Initializing parallel environment */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &ProcessID);
  MPI_Comm_size(MPI_COMM_WORLD, &Processes);

  /* Shared memory */
  int Size = atoi(argv[1]);
  int* Array = (int*)calloc(Size, sizeof(int));
  int Bound = atoi(argv[2]);

  /* Processing */
  int Min = 0, Max = 0;
  random(Array, Size, -Bound, Bound, &Min, &Max);

  /* Printing out result */
  if(ProcessID == 0) {
    printf("\n");
    printf(">> Result:");
    for(int index = 0; index < Size; index++) { printf(" %d", Array[index]); }
    printf("\n");
    printf(">> Min: %d - Max: %d", Min, Max);
    printf("\n");
  }

  /* Terminating parallel environment */
  MPI_Finalize();

  /* Procedure exit */
  return 0;
}
/*  */
