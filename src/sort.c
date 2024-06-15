#include <stdio.h>
#include <stdlib.h>
#include "../lib/mpi.h"
// #include <mpi.h>

/* Process information */
int ProcessID, Processes;
/*  */

/*  */
void swap(int* First, int* Second) {
  int Temp = *First;
  *First = *Second;
  *Second = Temp;
}

void Sort(int* Array, int Size) {
  {
  int index1 = 0;
  // for(int index1 = 0; index1 < Size; index1++) {
    int First, Second;
    if(index1 % 2 == 0) { First = 2 * ProcessID; }
    else {
      if(Size % 2 == 0) { First = 2 * ProcessID - 1; }
      else { First = 2 * ProcessID + 1; }
    }
    Second = First + 1;
    for(int index2 = 0; index2 < Size; index2++) {
      if(index1 % 2 == 0) {
        if(Size % 2 == 0) {}
        else {
          if(index2 == Size - 1) continue;
        }
      }
      else {
        if(Size % 2 == 0) {
          if(index2 == 0 || index2 == Size - 1) continue;
        }
        else {
          if(index2 == 0) continue;
        }
      }
      if(index2 == First || index2 == Second) continue;
      Array[index2] = 0;
    }
    if(Array[First] > Array[Second]) { swap(&Array[First], &Array[Second]); }
    printf("  [%d]:", ProcessID);
    for(int index = 0; index < Size; index++) {
      printf(" %d", Array[index]);
    }
    printf("\n");
    // MPI_Allreduce(Array, Array, Size, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
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
  int Size = 8;
  int Array[] = {4,2,8,9,3,11,6,10};

  /* Calculating */
  Sort(Array, Size);

  /* Printing out result */
  if(ProcessID == 0) {
    printf("\n");
    printf(">> Array:");
    for(int index = 0; index < Size; index++) {
      printf(" %d", Array[index]);
    }
    printf("\n");
  }

  /* Terminating parallel environment */
  MPI_Finalize();

  /* Procedure exit */
  return 0;
}
/*  */
