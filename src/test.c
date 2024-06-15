#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

int main() {
  // Set the seed for the random number generator
  srand(time(NULL));

  // Generate a random array
  int Size = 10;
  int Array[Size];
  int Min = 100, Max = -100;
  for (int index = 0; index < Size; index++) {
    Array[index] = rand() % 201 - 100;
    if(Array[index] < Min) { Min = Array[index]; }
    if(Array[index] > Max) { Max = Array[index]; }
  }
  QuickSort(Array, 0, Size - 1);

  // Generate a random number within the range of the array
  int Value = rand() % (Max - Min + 1) - 100;

  // Print the array and the chosen number
  printf("Array:");
  for (int index = 0; index < Size; index++) {
    printf(" %d", Array[index]);
  }
  printf("\nValue: %d\n", Value);

  return 0;
}