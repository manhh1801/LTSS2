#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../lib/mpi.h"

/* Process information */
int ProcessID, Processes;
/*  */

/* Time */
double TotalTime = 0;
double TotalTimeWithoutComp = 0;
double StartTime = 0, EndTime = 0;
double TimeFlag1 = 0, TimeFlag2 = 0;
/*  */

/* Random integer */
void Random(int* Array, int Size, int LowerBound, int UpperBound, int* Min, int* Max) {
  /* Setting seed for randomizing */
  if(ProcessID == 0) {
    TimeFlag1 = MPI_Wtime();
  }
  srand(time(NULL) + ProcessID * Size / Processes + UpperBound - LowerBound);
  if(ProcessID == 0) {
    TimeFlag2 = MPI_Wtime();
    TotalTimeWithoutComp += TimeFlag2 - TimeFlag1;
  }

  /* Processing */
  if(ProcessID == 0) {
    TimeFlag1 = MPI_Wtime();
  }
  if(Min != NULL) {
    *Min = UpperBound;
  }
  if(Max != NULL) {
    *Max = LowerBound;
  }
  int Range = UpperBound - LowerBound + 1;
  for(int index = ProcessID; index < Size; index += Processes) {
    Array[index] = rand() % Range + LowerBound;
    if(Min != NULL && Array[index] < *Min) {
      *Min = Array[index];
    }
    if(Max != NULL && Array[index] > *Max) {
      *Max = Array[index];
    }
  }
  if(ProcessID == 0) {
    TimeFlag2 = MPI_Wtime();
    TotalTimeWithoutComp += TimeFlag2 - TimeFlag1;
  }

  /* Merging data */
  for(int index = 0; index < Size; index++) {
    MPI_Allreduce(&Array[index], &Array[index], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  }
  if(Min != NULL) {
    MPI_Allreduce(Min, Min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
  }
  if(Max != NULL) {
    MPI_Allreduce(Max, Max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
  }
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
    if(ProcessID == 0) {
      TimeFlag1 = MPI_Wtime();
    }
    int Result = 0;
    if(ProcessID + Left < Right) {
      if(Array[ProcessID + Left] <= Target && Target <= Array[ProcessID + Left + 1]) {
        Result = ProcessID + Left;
      }
    }
    if(ProcessID == 0) {
      TimeFlag2 = MPI_Wtime();
      TotalTimeWithoutComp += TimeFlag2 - TimeFlag1;
    }
    MPI_Allreduce(&Result, &Result, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    return Result;
  }

  /* Finding the range of the target */
  else {
    if(ProcessID == 0) {
      TimeFlag1 = MPI_Wtime();
    }
    int Distances = Right - Left;
    int quotient = Distances / Processes, remainder = Distances % Processes;
    Left += quotient * ProcessID + (ProcessID < remainder ? ProcessID : remainder);
    Right = Left + quotient + (ProcessID < remainder ? 1 : 0);
    if(!(Array[Left] <= Target && Target <= Array[Right])) {
      Left = 0, Right = 0;
    }
    if(ProcessID == 0) {
      TimeFlag2 = MPI_Wtime();
      TotalTimeWithoutComp += TimeFlag2 - TimeFlag1;
    }
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
  if(ProcessID == 0) {
    StartTime = MPI_Wtime();
  }
  if(ProcessID == 0) {
    TimeFlag1 = MPI_Wtime();
  }
  int Size = atoi(argv[1]);
  int* Array = (int*)calloc(Size, sizeof(int));
  int Target = 0;
  int Bound = atoi(argv[2]);
  int Min = 0, Max = 0;
  if(ProcessID == 0) {
    TimeFlag2 = MPI_Wtime();
    TotalTimeWithoutComp += TimeFlag2 - TimeFlag1;
  }
  Random(Array, Size, -Bound, Bound, &Min, &Max);
  if(ProcessID == 0) {
    TimeFlag1 = MPI_Wtime();
  }
  QuickSort(Array, 0, Size - 1);
  if(ProcessID == 0) {
    TimeFlag2 = MPI_Wtime();
    TotalTimeWithoutComp += TimeFlag2 - TimeFlag1;
  }
  Random(&Target, 1, Min, Max, NULL, NULL);

  /* Calculating */
  int Result = Search(Array, 0, Size - 1, Target);
  if(ProcessID == 0) {
    EndTime = MPI_Wtime();
    TotalTime = EndTime - StartTime;
  }

  /* Printing out result */
  if(ProcessID == 0) {
    printf("\n");
    // printf(">> Array:");
    // for(int index = 0; index < Size; index++) {
    //   printf(" %d", Array[index]);
    // }
    // printf("\n");
    printf(">> Target: %d\n", Target);
    printf(">> Result = %d\n", Result);
    printf(">> Total time: %f(s), without comp: %f(s)\n", TotalTime, TotalTimeWithoutComp);
    printf("\n");
  }

  /* Terminating parallel environment */
  MPI_Finalize();

  /* Procedure exit */
  return 0;
}
/*  */
