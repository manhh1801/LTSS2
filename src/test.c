#include <stdio.h>

int main() {
  int Size = 20; int Processes = 5;
  int quotient = Size / Processes; int remainder = Size % Processes;
  for(int process = 0; process < Processes; process++) {
    int Start = quotient * process + (process < remainder ? process : remainder);
    int End = Start + (process < remainder ? quotient : quotient - 1);
    printf("%d: %d - %d\n", process, Start, End);
  }
}
