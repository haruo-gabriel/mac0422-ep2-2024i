#include "ep2.h"

double atualiza_tempo(struct timeval tempo_inicio, struct timeval tempo_atual) {
  gettimeofday(&tempo_atual, NULL);
  return (tempo_atual.tv_sec - tempo_inicio.tv_sec) + (tempo_atual.tv_usec - tempo_inicio.tv_usec)/1e6;
}

// int main(int argc, char *argv[]) {
//   int modo = -1;
//   pista = NULL;
//   ciclistas = NULL;

//   // Manipula a entrada
//   if (argc == 3) {
//     modo = 0; // Modo padrão
//   } else if (argc == 4) {
//     modo = 1; // Modo debug
//   } else {
//     printf("Uso: ./%s <d> <k> [-debug] \n", argv[0]);
//     return 1;
//   }
//   g_num_ciclistas = g_num_ciclistas_vivos = argv[2];
//   g_tamanho_pista = argv[3];

//   // Aloca memória para a pista e ciclistas
//   pista = cria_pista();
//   if (pista == NULL){
//     perror("Erro ao criar pistas");
//     return 1;
//   }
//   ciclistas = cria_ciclistas();
//   if (cria_threads() == NULL) {
//     perror("Erro ao criar threads");
//     return 1;
//   }

//   // Inicia o clock
//   gettimeofday(&tempo_inicio, NULL);

//   // Loop principal
//   while (g_num_ciclistas_vivos <= 2) {
//     avanca_ciclistas();
//     // Avança um tick do clock
//     // Imprime as informações na tela
//     imprime_corrida();
//   }

//   return 0;
// }