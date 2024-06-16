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

/* Quick sort */
void swap(int* First, int* Second) {
  int Temp = *First;
  *First = *Second;
  *Second = Temp;
}

void QuickSort(int* Array, int Left, int Right) {
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
    QuickSort(Array, Left, Checkpoint - 1);
    QuickSort(Array, Checkpoint + 1, Right);
  }
}
/*  */

/* Parallel search */
int Search(int* Array, int Left, int Right, int Target) {
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
    return Search(Array, Left, Right, Target);
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
  int Min = 0, Max = 0;
  Random(Array, Size, -Bound, Bound, &Min, &Max);
  int Target = 0;
  Random(&Target, 1, Min, Max, NULL, NULL);

  /* Calculating */
  int Result = Search(Array, 0, Size - 1, Target);

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
