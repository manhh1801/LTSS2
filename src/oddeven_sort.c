#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

/* Process information */
int ProcessID, Processes;
/*  */

/* Odd-even sort */
void swap(int* First, int* Second) {
  int Temp = *First;
  *First = *Second;
  *Second = Temp;
}

void oddeven_sort(int* Array, int Size) {
  for(int index1 = 0; index1 < Size; index1++) {
    /* Defining the choke points */
    int First = index1 % 2 == 0 ? 2 * ProcessID : Size % 2 == 0 ? 2 * ProcessID - 1: 2 * ProcessID + 1, Second = First + 1;

    /* Preprocessing the array */
    for(int index2 = 0; index2 < Size; index2++) {
      if(index1 % 2 == 0) { if(Size % 2 != 0) { if(index2 == Size - 1) { continue; } } }
      else {
        if(Size % 2 == 0) { if(index2 == 0 || index2 == Size - 1) { continue; } }
        else { if(index2 == 0) { continue; } }
      }
      if(index2 == First || index2 == Second) { continue; }
      Array[index2] = -101;
    }

    /* Processing */
    if(index1 % 2 != 0 && Size % 2 == 0) { if(ProcessID != 0) { if(Array[First] > Array[Second]) { swap(&Array[First], &Array[Second]); } } }
    else { if(Array[First] > Array[Second]) { swap(&Array[First], &Array[Second]); } }
    for(int index2 = 0; index2 < Size; index2++) { MPI_Allreduce(&Array[index2], &Array[index2], 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD); }
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
  oddeven_sort(Array, Size);

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
