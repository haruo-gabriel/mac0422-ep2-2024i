#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
 
 
 
// Structs
typedef struct {
  int id;
  int posicao_x, posicao_y;
  int velocidade;
  int voltas, colocacao;
  bool na_corrida;
  bool quebrou;
  bool ganhou;
  bool atualizou_posicao, atualizou_velocidade;
} Ciclista;

typedef struct {
  int id;
} ThreadArgs;


// Variáveis globais
int g_num_ciclistas;
int g_num_voltas_completas;
int g_tamanho_pista;
long long int clock_ms;
int ** pista;
Ciclista** ciclistas;
pthread_t *threads;
pthread_mutex_t* mutex_pista;
pthread_mutex_t mutex_ciclistas;
pthread_barrier_t barreira_1, barreira_2;



// Protótipos
void cria_mutex_pista(int);
void destroi_mutex_pista(int);
// void cria_mutex_ciclistas(int);
// void destroi_mutex_ciclistas(int);
void cria_pista(int);
void destroi_pista(int);
void cria_ciclistas(int);
Ciclista* cria_ciclista(int);
void destroi_ciclistas(int);
void cria_threads(int);
void* f_ciclista(void*);
void atualiza_posicao(int, int, int);
bool avanca_pra_frente(int, int, int);
bool ultrapassa_por_cima(int, int, int);
bool ultrapassa_por_baixo(int, int, int);
void atualiza_velocidade(int);
bool quebra_ciclista();
void imprime_corrida(int, int*, int*);