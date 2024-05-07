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
  double tempo_final;
  bool na_corrida;
  bool quebrou;
  bool ganhou;
  bool atualizou_posicao;
  // bool atualizou_velocidade;
} Ciclista;

typedef struct {
  int id;
} ThreadArgs;


// Variáveis globais
struct timespec inicio, agora;
FILE* saida;

Ciclista** ciclistas;
int g_num_ciclistas;
int g_num_na_corrida;
int g_num_quebrados;
int g_num_vencedores;
pthread_t* threads;

int** pista;
int g_tamanho_pista;
int g_num_voltas;
int g_num_voltas_para_ganhar;

// pthread_mutex_t* mutex_pista;
pthread_mutex_t mutex_pista;
pthread_mutex_t mutex_ciclistas;
pthread_mutex_t mutex_voltas;

pthread_barrier_t barreira_ciclistas_andaram;
pthread_barrier_t barreira_impressao;
// pthread_barrier_t barreira_3;



// Protótipos
void inicializa_clock();

void cria_mutexes_barreiras();
void destroi_mutexes_barreiras();

void cria_ciclistas();
Ciclista* cria_ciclista(int);
void destroi_ciclistas();

void cria_pista();
void destroi_pista();

void cria_threads();
void destroi_threads();

void* f_ciclista(void*);
int pode_ultrapassar(int);
void atualiza_posicao(int, int, int);
void avanca_pra_frente(int, int, int);
void ultrapassa_por_cima(int, int, int);
void ultrapassa_por_baixo(int, int, int);
void atualiza_velocidade(int);
bool quebra_ciclista(int);

void imprime_corrida_debug(int*, int*);
void imprime_corrida();
void imprime_pista();
void imprime_pista_invertida();