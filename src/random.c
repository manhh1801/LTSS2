#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

/* Process information */
int ProcessID, Processes;
/*  */

/* Random integer */
void Random(int* Array, int Size, int LowerBound, int UpperBound, int* Min, int* Max) {
  /* Setting seed for randomizing */
  srand(time(NULL) + ProcessID * Size / Processes + UpperBound - LowerBound);

  /* Processing */
  if(Min != NULL) { *Min = UpperBound; }
  if(Max != NULL) { *Max = LowerBound; }
  int Range = UpperBound - LowerBound + 1;
  for(int index = ProcessID; index < Size; index += Processes) {
    Array[index] = rand() % Range + LowerBound;
    if(Min != NULL && Array[index] < *Min) { *Min = Array[index]; }
    if(Max != NULL && Array[index] > *Max) { *Max = Array[index]; }
  }
  for(int index = 0; index < Size; index++) { MPI_Allreduce(&Array[index], &Array[index], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD); }
  if(Min != NULL) { MPI_Allreduce(Min, Min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD); }
  if(Max != NULL) { MPI_Allreduce(Max, Max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD); }
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
  Random(Array, Size, -Bound, Bound, &Min, &Max);

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
