#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

/* Process information */
int ProcessID, Processes;
/*  */

/* Merge sort */
void
/*  */

/* Entry procedure */
int main(int argc, char* argv[]) {
  /* Initializing parallel environment */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &ProcessID);
  MPI_Comm_size(MPI_COMM_WORLD, &Processes);

  /* Shared memory */
  int Size = 2 * Processes + rand() % 2;
  int* Array = (int*)calloc(Size, sizeof(int));
  if(ProcessID == 0) {
    srand(time(NULL));
    for(int index = 0; index < Size; index++) { Array[index] = rand() % 201 - 100; }
  }
  for (int index = 0; index < Size; index++) { MPI_Allreduce(&Array[index], &Array[index], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD); }
  if(ProcessID == 0) {
    printf("\n");
    printf(">> Array:");
    for(int index = 0; index < Size; index++) { printf(" %d", Array[index]); }
  }

  /* Calculating */
  sort(Array, Size);

  /* Printing out result */
  if(ProcessID == 0) {
    printf("\n");
    printf(">> Result:");
    for(int index = 0; index < Size; index++) { printf(" %d", Array[index]); }
    printf("\n");
  }

  /* Terminating parallel environment */
  MPI_Finalize();

  /* Procedure exit */
  return 0;
}
/*  */
