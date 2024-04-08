#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct {
  unsigned int posicao;
  unsigned int voltas;
} Ciclista;

typedef struct {
  unsigned int tamanho;
  unsigned int num_lanes;
  unsigned int **matriz;
} Pista;

// Variáveis globais
// unsigned int g_num_ciclistas;
// unsigned int g_num_ciclistas_vivos;
// unsigned int g_tamanho_pista;
// struct timeval tempo_inicio, tempo_atual;
// Pista* pista;
// Ciclista** ciclistas;

// Protótipos
// Pista* cria_pista();
// Ciclista** cria_threads();
double atualiza_tempo(struct timeval, struct timeval);
// int imprime_corrida();
