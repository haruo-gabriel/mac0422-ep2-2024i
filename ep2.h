#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
 
 
 
// Structs
typedef struct {
  int id;
  int posicao_x, posicao_y;
  int velocidade;
  int voltas;
  int colocacao;
  bool na_corrida;
  bool quebrou;
  bool ganhou;
  bool atualizou_posicao;
  bool atualizou_velocidade;
} Ciclista;

typedef struct {
  int id;
} ThreadArgs;


// Variáveis globais
long long int clock_ms;

Ciclista** ciclistas;
int g_num_ciclistas;
int g_num_na_corrida;
int g_num_quebrados;
int g_num_vencedores;
pthread_t *threads;

int ** pista;
int g_tamanho_pista;
int g_num_voltas_completas;

// pthread_mutex_t* mutex_pista;
pthread_mutex_t mutex_pista;
pthread_mutex_t mutex_ciclistas;
pthread_mutex_t mutex_voltas;

pthread_barrier_t barreira_ciclistas_andaram;
pthread_barrier_t barreira_impressao;
pthread_barrier_t barreira_3;



// Protótipos
void cria_mutexes_barreiras();
void destroi_mutexes_barreiras();

void cria_ciclistas();
Ciclista* cria_ciclista();
void destroi_ciclistas();

void cria_pista();
void destroi_pista();

void cria_threads();
void destroi_threads();

void* f_ciclista(void*);
void atualiza_posicao();
bool avanca_pra_frente();
bool ultrapassa_por_cima();
bool ultrapassa_por_baixo();
void atualiza_velocidade();
bool quebra_ciclista();

void imprime_corrida(int, int*, int*);
void imprime_pista();