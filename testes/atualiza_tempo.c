#include "../ep2.h"

struct timeval tempo_inicio, tempo_atual;

int main() {
  gettimeofday(&tempo_inicio, NULL);
  while (1) {
    printf("Tempo atual: %f\n", atualiza_tempo(tempo_inicio, tempo_atual));
    sleep(1);
  }
  return 0;
}