#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

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

/* Odd-even sort */
void swap(int* First, int* Second) {
  int temp = *First;
  *First = *Second;
  *Second = temp;
}

void OddEvenSort(int* Array, int Size) {
  for(int index1 = 0; index1 < Size; index1++) {
    if(ProcessID == 0) {
      TimeFlag1 = MPI_Wtime();
    }
    int* Tasks = NULL;
    int TaskSize = 0;
    for(int index2 = ProcessID; index2 < Size / 2 - (index1 % 2 != 0 && Size % 2 == 0 ? 1 : 0); index2 += Processes) {
      Tasks = (int*)realloc(Tasks, (TaskSize + 1) * sizeof(int));
      Tasks[TaskSize] = 2 * index2 + index1 % 2 == 0 ? 0 : 1;
      TaskSize += 1;
    }

    int* temp = (int*)calloc(Size, sizeof(int));
    for(int task = 0; task < TaskSize; task++) {
      int First = Tasks[task], Second = First + 1;
      temp[First] = Array[First], temp[Second] = Array[Second];
      if(index1 % 2 == 0) { if(Size % 2 != 0) { if(ProcessID == 0) { temp[Size - 1]  = Array[Size - 1]; } } }
      else {
        if(Size % 2 == 0) { if(ProcessID == 0) { temp[0] = Array[0], temp[Size - 1]  = Array[Size - 1]; }}
        else { if(ProcessID == 0) { temp[0] = Array[0]; } }
      }

      if(temp[First] > temp[Second]) { swap(&temp[First], &temp[Second]); }
    }

    if(ProcessID == 0) {
      TimeFlag2 = MPI_Wtime();
      TotalTimeWithoutComp += TimeFlag2 - TimeFlag1;
    }
    for(int index2 = 0; index2 < Size; index2++) { MPI_Allreduce(&temp[index2], &Array[index2], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD); }
    free(temp);

    free(Tasks);
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
  int Bound = atoi(argv[2]);
  if(ProcessID == 0) {
    TimeFlag2 = MPI_Wtime();
    TotalTimeWithoutComp += TimeFlag2 - TimeFlag1;
  }
  Random(Array, Size, -Bound, Bound, NULL, NULL);
  if(ProcessID == 0) {
    printf("\n");
    printf(">> Array:");
    for(int index = 0; index < Size; index++) { printf(" %d", Array[index]); }
    printf("\n");
  }

  /* Calculating */
  OddEvenSort(Array, Size);
  if(ProcessID == 0) {
    EndTime = MPI_Wtime();
    TotalTime = EndTime - StartTime;
  }

  /* Printing out result */
  if(ProcessID == 0) {
    printf("\n");
    printf(">> Result:");
    for(int index = 0; index < Size; index++) { printf(" %d", Array[index]); }
    printf("\n");
    printf(">> Total time: %f(s), without comp: %f(s)\n", TotalTime, TotalTimeWithoutComp);
    printf("\n");
  }

  /* Terminating parallel environment */
  MPI_Finalize();

  /* Procedure exit */
  return 0;
}
/*  */
