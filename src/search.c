#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

/* Process information */
int ProcessID, Processes;
/*  */

/* Quick sort */
void swap(int* First, int* Second) {
  int Temp = *First;
  *First = *Second;
  *Second = Temp;
}

void sort(int* Array, int Left, int Right) {
  int Pivot, Checkpoint;
  if(Left < Right) {
    Pivot = Array[Left];
    Checkpoint = Left;
    for(int index = Left + 1; index <= Right; index++) {
      if(Array[index] < Pivot) {
        swap(&Array[++Checkpoint], &Array[index]);
      }
    }
    swap(&Array[Checkpoint], &Array[Left]);
    sort(Array, Left, Checkpoint - 1);
    sort(Array, Checkpoint + 1, Right);
  }
}
/*  */

/* Parallel search */
int search(int* Array, int Left, int Right, int Target) {
  /* Finding the place of the target */
  if(Right - Left <= Processes) {
    int Result = 0;
    if(ProcessID + Left < Right) { if(Array[ProcessID + Left] <= Target && Target <= Array[ProcessID + Left + 1]) { Result = ProcessID + Left; } }
    MPI_Allreduce(&Result, &Result, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    return Result;
  }

  /* Finding the range of the target */
  else {
    int Distances = Right - Left;
    int quotient = Distances / Processes, remainder = Distances % Processes;
    Left += quotient * ProcessID + (ProcessID < remainder ? ProcessID : remainder);
    Right = Left + quotient + (ProcessID < remainder ? 1 : 0);
    if(!(Array[Left] <= Target && Target <= Array[Right])) { Left = 0, Right = 0; }
    MPI_Allreduce(&Left, &Left, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&Right, &Right, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    return search(Array, Left, Right, Target);
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
  int Target = 0;
  if(ProcessID == 0) {
    srand(time(NULL));
    int Min = 100, Max = -100;
    for (int index = 0; index < Size; index++) {
      Array[index] = rand() % 201 - 100;
      if(Array[index] < Min) { Min = Array[index]; }
      if(Array[index] > Max) { Max = Array[index]; }
    }
    sort(Array, 0, Size - 1);
    Target = rand() % (Max - Min + 1) + Min;
  }
  for (int index = 0; index < Size; index++) { MPI_Allreduce(&Array[index], &Array[index], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD); }
  MPI_Allreduce(&Target, &Target, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  /* Calculating */
  int Result = search(Array, 0, Size - 1, Target);

  /* Printing out result */
  if(ProcessID == 0) {
    printf("\n");
    printf(">> Array:");
    for(int index = 0; index < Size; index++) { printf(" %d", Array[index]); }
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
