#include <stdio.h>

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
  int Size = 8;
  int Array[] = {4,2,8,9,3,11,6,10};
  QuickSort(Array, 0, 7);
  for(int index = 0; index < Size; index++) {
    printf(" %d", Array[index]);
  }
  printf("\n");
  return 0;
}
