#include <stdio.h>
#include <stdlib.h>
#include "../lib/mpi.h"

/* Process information */
int ProcessID, Processes;
/*  */

int ParallelSearch(int* Array, int Left, int Right, int Target) {
  if(Right - Left <= Processes) {
    int Result = 0;
    if(Array[ProcessID] <= Target && Target <= Array[ProcessID + 1]) { Result = ProcessID; }
    MPI_Allreduce(&Result, &Result, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    return Result;
  }
  else {
    int Tasks = Right - Left + 1;
    int quotient = Tasks / Processes, remainder = Tasks % Processes;
    int Start = quotient * ProcessID + (ProcessID < remainder ? ProcessID : remainder);
    int End = Start + (ProcessID < remainder ? quotient : quotient - 1);
    Left = 0; Right = 0;
    if(Array[Start] <= Target && Target <= Array[End]) { Left = Start; Right = End; }
    MPI_Allreduce(&Left, &Left, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&Right, &Right, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    return ParallelSearch(Array, Left, Right, Target);
  }
}

/* Entry procedure */
int main(int argc, char* argv[]) {
  /* Initializing parallel environment */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &ProcessID);
  MPI_Comm_size(MPI_COMM_WORLD, &Processes);

  /* Shared memory */
  int Size = atoi(argv[1]);
  int Array[Size];
  for(int index = 0; index < Size; index++) {
    Array[index] = 2 * index;
  }

  int Target = atoi(argv[2]);

  // /* Calculating */
  // int Result = 0;
  // if(Array[ProcessID] <= Value && Value <= Array[ProcessID + 1]) { Result = ProcessID; }
  // MPI_Allreduce(&Result, &Result, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

  /* Calculating */
  int Result = ParallelSearch(Array, 0, Size, Target);

  /* Printing out result */
  if(ProcessID == 0) { printf("\nResult = %d\n\n", Result); }

  /* Terminating parallel environment */
  MPI_Finalize();

  /* Procedure exit */
  return 0;
}
/*  */