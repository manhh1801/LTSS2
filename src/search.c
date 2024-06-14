#include <stdio.h>
#include <stdlib.h>
#include "../lib/mpi.h"

/* Process information */
int ProcessID, Processes;
/*  */

/* Entry procedure */
int main(int argc, char* argv[]) {
  /* Initializing parallel environment */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &ProcessID);
  MPI_Comm_size(MPI_COMM_WORLD, &Processes);

  /* Shared memory */

  int Array[] = {2,4,6,8,10,12,14,16,18,20};
  int Value = 9;

  /* Calculating */
  int Result = 0;
  if((Array[ProcessID] <= Value) && (Value <= Array[ProcessID + 1])) { Result = ProcessID; }
  MPI_Allreduce(&Result, &Result, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  /* Printing out result */
  if(ProcessID == 0) { printf("\nResult = %d\n", Result); }

  /* Terminating parallel environment */
  MPI_Finalize();

  /* Procedure exit */
  return 0;
}
/*  */