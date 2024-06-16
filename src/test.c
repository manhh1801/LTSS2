#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int* First, int* Second) {
  int Temp = *First;
  *First = *Second;
  *Second = Temp;
}

void quick_sort(int* Array, int Left, int Right) {
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
    quick_sort(Array, Left, Checkpoint - 1);
    quick_sort(Array, Checkpoint + 1, Right);
  }
}

int merge_sort(int* Array ,int* Array1, int Size1, int* Array2, int Size2) {
  int Size = Size1 + Size2;
  int index1 = 0, index2 = 0;
  for(int index = 0; index < Size; index++) {
    if(index1 == Size1) {
      Array[index] = Array2[index2++];
      continue;
    }
    if(index2 == Size2) {
      Array[index] = Array1[index1++];
      continue;
    }
    if(Array1[index1] < Array2[index2]) { Array[index] = Array1[index1++]; }
    else { Array[index] = Array2[index2++]; }
  }
  return Size;
}

int main() {
  // Set the seed for the random number generator
  srand(time(NULL));

  // Generate random arrays
  int Size1 = 10;
  int Array1[Size1];
  for(int index1 = 0; index1 < Size1; index1++) { Array1[index1] = rand() % 201 - 100; }
  quick_sort(Array1, 0, Size1 - 1);
  int Size2 = 10;
  int Array2[Size2];
  for(int index2 = 0; index2 < Size2; index2++) { Array2[index2] = rand() % 201 - 100; }
  quick_sort(Array2, 0, Size2 - 1);

  // Initialize result array
  int* Array = (int*)calloc(Size1 + Size2, sizeof(int));
  int Size = merge_sort(Array, Array1, Size1, Array2, Size2);

  // Print the arrays
  printf("Array1:");
  for (int index1 = 0; index1 < Size1; index1++) { printf(" %d", Array1[index1]); }
  printf("\n");
  printf("Array2:");
  for (int index2 = 0; index2 < Size2; index2++) { printf(" %d", Array2[index2]); }
  printf("\n");
  printf("Array:");
  for (int index = 0; index < Size; index++) { printf(" %d", Array[index]); }
  printf("\n");

  return 0;
}