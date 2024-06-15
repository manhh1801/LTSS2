#include <stdio.h>

int main() {
  // int Size = 20; int Processes = 5;
  // int quotient = Size / Processes; int remainder = Size % Processes;
  // for(int process = 0; process < Processes; process++) {
  //   int Start = quotient * process + (process < remainder ? process : remainder);
  //   int End = Start + (process < remainder ? quotient : quotient - 1);
  //   printf("%d: %d - %d\n", process, Start, End);
  // }

  int Distances = 17, Processes = 5;
  int quotient = Distances / Processes; int remainder = Distances % Processes;
  for(int process = 0; process < Processes; process++) {
    int Distance = quotient + (process < remainder ? 1 : 0);
    int Start = quotient * process + (process < remainder ? process : remainder);
    int End = Start + Distance;
    printf("%d: %d - %d\n", process, Start, End);
  }
}
