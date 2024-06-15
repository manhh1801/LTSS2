#include <stdio.h>
#include <stdlib.h>
// #include "../lib/mpi.h"
#include <mpi.h>

/* Process information */
int ProcessID, Processes;
/*  */

int ParallelSearch(int* Array, int Left, int Right, int Target) {
  if(Right - Left <= Processes) {
    int Result = 0;
    if(ProcessID + Left < Right) {
      if(Array[ProcessID + Left] <= Target && Target <= Array[ProcessID + Left + 1]) {
        Result = ProcessID + Left;
      }
    }
    MPI_Allreduce(&Result, &Result, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    return Result;
  }
  else {
    int Distances = Right - Left;
    int quotient = Distances / Processes, remainder = Distances % Processes;
    Left += quotient * ProcessID + (ProcessID < remainder ? ProcessID : remainder);
    Right = Left + quotient + (ProcessID < remainder ? 1 : 0);
    if(!(Array[Left] <= Target && Target <= Array[Right])) {
      Left = 0;
      Right = 0;
    }
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

  /* Calculating */
  int Result = ParallelSearch(Array, 0, Size - 1, Target);

  /* Printing out result */
  if(ProcessID == 0) {
    printf("\n");
    printf(">> Array:");
    for(int index = 0; index < Size; index++) {
      printf(" %d", Array[index]);
    }
    printf("\n");
    printf(">> Target: %d\n", Target);
    printf(">> Result = %d\n", Result);
    printf("\n");
  }

  /* Terminating parallel environment */
  MPI_Finalize();

  /* Procedure exit */
  return 0;
}
/*  */
