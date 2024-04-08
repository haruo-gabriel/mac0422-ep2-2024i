#include <stdio.h>

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
unsigned int g_num_ciclistas;
unsigned int g_num_ciclistas_vivos;
unsigned int g_tamanho_pista;
Pista* pista;
Ciclista** ciclistas;

// Protótipos
Pista* cria_pista();
Ciclista** cria_threads();
int imprime_corrida();
