#include "ep2.h"

void cria_mutexes_barreiras() {
  pthread_mutex_init(&mutex_pista, NULL);
  pthread_mutex_init(&mutex_ciclistas, NULL);
  pthread_mutex_init(&mutex_voltas, NULL);
  pthread_barrier_init(&barreira_ciclistas_andaram, NULL, g_num_ciclistas+1);
  pthread_barrier_init(&barreira_impressao, NULL, g_num_ciclistas+1);
  pthread_barrier_init(&barreira_3, NULL, g_num_ciclistas+1);
  return;
}

void destroi_mutexes_barreiras() {
  pthread_mutex_destroy(&mutex_pista);
  pthread_mutex_destroy(&mutex_ciclistas);
  pthread_mutex_destroy(&mutex_voltas);
  pthread_barrier_destroy(&barreira_ciclistas_andaram);
  pthread_barrier_destroy(&barreira_impressao);
  pthread_barrier_destroy(&barreira_3);
  return;
}

void cria_pista() {
  // Aloca memória para a pista
  pista = (int**) malloc(g_tamanho_pista * sizeof(int*));
  for (int i = 0; i < g_tamanho_pista; i++) {
    pista[i] = (int*) malloc(10 * sizeof(int));
  }
  if (pista == NULL) {
    perror("Erro ao alocar memória para a pista");
    return;
  }

  // Inicializa a matriz da pista
  for (int i = 0; i < g_tamanho_pista; i++) {
    for (int j = 0; j < 10; j++) {
      pista[i][j] = -1;
    }
  }
}

void destroi_pista() {
  for (int i = 0; i < 10; i++) {
    free(pista[i]);
  }
  free(pista);
}

void cria_ciclistas() {
  // Aloca memória para o vetor de ciclistas
  ciclistas = (Ciclista**) malloc(g_num_ciclistas * sizeof(Ciclista*));
  if (ciclistas == NULL) {
    perror("Erro ao alocar memória para os ciclistas");
    return;
  }

  // Cria os objetos Ciclista
  for (int i = 0; i < g_num_ciclistas; i++) {
    ciclistas[i] = cria_ciclista(i);
  }
  return;
}

void destroi_ciclistas() {
  for (int i = 0; i < g_num_ciclistas; i++) {
    free(ciclistas[i]);
  }
  free(ciclistas);
  return;
}

Ciclista* cria_ciclista(int id) {
  Ciclista* ciclista = (Ciclista*) malloc(sizeof(Ciclista));
  ciclista->id = id;
  ciclista->voltas = 0;
  ciclista->posicao_x = -1;
  ciclista->posicao_y = -1;
  ciclista->colocacao = -1;
  ciclista->velocidade = 30; // 30 km/h ou 1m/120ms
  ciclista->quebrou = false;
  ciclista->ganhou = false;
  ciclista->na_corrida = true;
  ciclista->atualizou_posicao = false;
  ciclista->atualizou_velocidade = false;

  return ciclista;
}

void cria_threads () {
  threads = (pthread_t*) malloc(g_num_ciclistas * sizeof(pthread_t));
  for (int i = 0; i < g_num_ciclistas; i++) {
    ThreadArgs* arg = (ThreadArgs*) malloc(sizeof(ThreadArgs));
    arg->id = i;
    pthread_create(&threads[i], NULL, f_ciclista, (void*)arg);
  }
}

void destroi_threads() {
  for (int i = 0; i < g_num_ciclistas; i++) {
    pthread_join(threads[i], NULL);
  }
  free(threads);
  return;
}

void cria_largada() {
  // // Cria um vetor para aleatorizar a ordem de largada
  // int* array = malloc(num_ciclistas * sizeof(int));
  // for(int i = 0; i < num_ciclistas; i++) {
  //   array[i] = i;
  // }
  // for (int i = num_ciclistas-1; i > 0; i--) {
  //   int j = rand() % (i+1);
  //   int temp = array[i];
  //   array[i] = array[j];
  //   array[j] = temp;
  // }

  int colunas = (g_num_ciclistas + 4) / 5; // quantas colunas de ciclistas teremos
  int i = 0; // contador para o array de ciclistas
  for (int x = 0; x < colunas && i < g_num_ciclistas; x++) {
    for (int y = 0; y < 5 && i < g_num_ciclistas; y++) {
      pista[x][y] = ciclistas[i]->id;
      ciclistas[i]->posicao_x = x;
      ciclistas[i]->posicao_y = y;
      // ciclistas[array[i]]->posicao_x = x;
      // ciclistas[array[i]]->posicao_y = y;
      i++;
    }
  }

  // free(array);

  return;
}

void* f_ciclista(void* arg) {
  int id = ((ThreadArgs*)arg)->id;
  free(arg);
  Ciclista* ciclista = ciclistas[id];
  int x, y;
  bool sai_do_loop = false;

  // Loop da volta 0
  while (ciclista->voltas == 0) {
    x = ciclista->posicao_x;
    y = ciclista->posicao_y;
    atualiza_posicao(id, x, y);
    pthread_barrier_wait(&barreira_ciclistas_andaram);
    // printf("%d: passei da barreira 1\n", id);
    pthread_barrier_wait(&barreira_impressao);
    // printf("%d: passei da barreira 2\n", id);
  }

  while (!sai_do_loop) {
    x = ciclista->posicao_x;
    y = ciclista->posicao_y;

    // Atualiza a posição do ciclista na pista
    atualiza_posicao(id, x, y);

    // Atualiza velocidade do ciclista
    if (x == 0) {
      atualiza_velocidade(id);
    }

    // Verifica se o ciclista ganhou
    if (ciclista->ganhou) {
      sai_do_loop = true;
    }
    // Verifica se o ciclista quebrou
    if (ciclista->voltas % 6 == 0 && quebra_ciclista(id)) {
      ciclista->quebrou = true;
      sai_do_loop = true;
    }
    if (sai_do_loop) {
      // Tira o ciclista da da pista
      pthread_mutex_lock(&mutex_pista);
      pista[x][y] = -1;
      pthread_mutex_unlock(&mutex_pista);
    }

    // printf("%d: Esperando barreira 1\n", id);
    pthread_barrier_wait(&barreira_ciclistas_andaram);
    // printf("%d: Passei da barreira 1\n", id);
    // printf("%d: Esperando barreira 2\n", id);
    pthread_barrier_wait(&barreira_impressao);
    // printf("%d: Passei da barreira 2\n", id);
  }

  // Aguarda todos os ciclistas encerrarem
  while (g_num_na_corrida != 0) {
    // printf("%d: Esperando barreira 1\n", id);
    pthread_barrier_wait(&barreira_ciclistas_andaram);
    // printf("%d: Passei da barreira 1\n", id);
    // printf("%d: Esperando barreira 2\n", id);
    pthread_barrier_wait(&barreira_impressao);
    // printf("%d: Passei da barreira 2\n", id);
  }

  pthread_exit(NULL);
}

void atualiza_posicao(int id, int x, int y) {
  Ciclista* ciclista = ciclistas[id];
  if ((ciclista->velocidade == 60) ||
      (ciclista->velocidade == 30 && !ciclista->atualizou_posicao))
  {
    pthread_mutex_lock(&mutex_pista);
    // Se há um ciclista à frente
    if (!avanca_pra_frente(id, x, y)) {
      // Se o ciclista está na borda inferior
      if (y == 0) {
        ultrapassa_por_cima(id, x, y);
      }
      // Se o ciclista está na borda superior 
      else if (y == 9) {
        ultrapassa_por_baixo(id, x, y);
      }
      // Se o ciclista está no meio da pista
      else {
        // Sorteia se o ciclista vai tentar ultrapassar primeiro por cima ou por baixo
        if (rand() % 2 == 0) {
          if (!ultrapassa_por_cima(id, x, y))
            ultrapassa_por_baixo(id, x, y);
        }
        else {
          if (!ultrapassa_por_baixo(id, x, y)) {
            ultrapassa_por_cima(id, x, y);
          }
        }
      }
    }
    pthread_mutex_unlock(&mutex_pista);
  }
  else {
    ciclista->atualizou_posicao = false;
  }
}

bool avanca_pra_frente(int id, int x, int y) {
  Ciclista* ciclista = ciclistas[id];
  int x_prox = (x+1) % g_tamanho_pista;
  if (pista[x_prox][y] == -1) {
    // Verifica se o ciclista completou uma volta
    if (x_prox == 0) {
      ciclista->voltas++;
      // Verifica se o ciclista ganhou
      if (ciclista->voltas == g_num_voltas_completas) {
        ciclista->ganhou = true;
        pista[x][y] = -1;
        ciclista->atualizou_posicao = true;
        return true;
      }
    }
    // Ciclista deixa a posição atual livre
    pista[x][y] = -1;
    // Ciclista avança e ocupa a posição à frente
    pista[x_prox][y] = id;
    ciclista->posicao_x = x_prox;
    ciclista->atualizou_posicao = true;
    return true;
  }
  ciclista->atualizou_posicao = false;
  return false;
}

bool ultrapassa_por_cima(int id, int x, int y) {
  // printf("%d: ultrapassa_por_cima\n", id);
  Ciclista* ciclista = ciclistas[id];
  int x_prox = (x+1) % g_tamanho_pista;
  if (pista[x_prox][y+1] == -1) {
    // Ciclista deixa a posição atual livre
    pista[x][y] = -1;
    // Ciclista ultrapassa e ocupa a posição à diagonal
    pista[x_prox][y+1] = id;
    ciclista->posicao_x = x_prox;
    ciclista->posicao_y = y+1;

    // Verifica se o ciclista completou uma volta
    if (x_prox == 0) {
      ciclista->voltas++;
    }
    ciclista->atualizou_posicao = true;
    return true;
  }

  ciclista->atualizou_posicao = false;
  return false;
}

bool ultrapassa_por_baixo(int id, int x, int y) {
  // printf("%d: ultrapassa_por_baixo\n", id);
  Ciclista* ciclista = ciclistas[id];
  int x_prox = (x+1) % g_tamanho_pista;

  if (pista[x_prox][y-1] == -1) {
    // Ciclista deixa a posição atual livre
    pista[x][y] = -1;
    // Ciclista ultrapassa e ocupa a posição à diagonal
    pista[x_prox][y-1] = id;
    ciclista->posicao_x = x_prox;
    ciclista->posicao_y = y-1;

    // Verifica se o ciclista completou uma volta
    if (x_prox == 0) {
      ciclista->voltas++;
    }
    ciclista->atualizou_posicao = true;
    return true;
  }
    ciclista->atualizou_posicao = false;
  return false;
}

void atualiza_velocidade(int id) {
  Ciclista* ciclista = ciclistas[id];
  int x, y;

  pthread_mutex_lock(&mutex_ciclistas);

  // Sorteia a velocidade do ciclista
  if (ciclista->velocidade == 30) {
    // 70% de chance de aumentar a velocidade para 70 km/h
    if (rand() % 100 < 30)
      ciclista->velocidade = 30;
    else
      ciclista->velocidade = 60;
  } else if (ciclista->velocidade == 60) {
    // 50% de chance de aumentar a velocidade para 70 km/h
    if (rand() % 100 < 50)
      ciclista->velocidade = 30;
    else
      ciclista->velocidade = 60;
  }

  // Atualiza a velocidade de todos os ciclistas que estão
  // imediatamente atrás e que não podem ultrapassar
  // if (ciclista->velocidade == 30) {
  //   x = g_tamanho_pista + (ciclista->posicao_x-1);
  //   printf("x=%d\n", x);
  //   y = ciclista->posicao_y;
  //   while (pista[x][y] != -1) {
  //     ciclistas[pista[x][y]]->velocidade = 30;
  //     x = g_tamanho_pista + (x-1);
  //   }
  // }

  pthread_mutex_unlock(&mutex_ciclistas);

  return;
}

bool quebra_ciclista(int id) {
  if (rand() % 100 < 15) {  // 15% de chance de quebrar
    ciclistas[id]->quebrou = true;
    return true;
  }
  return false;
}

void imprime_corrida(int modo, int* vencedores, int* quebrados) {
  if (modo == 0) { // Modo normal
  } else { // Modo debug
    imprime_pista();
    printf("\n");
    // Imprime variáveis globais
    printf("g_num_voltas_completas: %d\n", g_num_voltas_completas);
    printf("g_num_ciclistas: %d\n", g_num_ciclistas); 
    printf("g_num_na_corrida: %d\n", g_num_na_corrida);
    printf("g_num_vencedores: %d\n", g_num_vencedores);
    printf("g_num_quebrados: %d\n", g_num_quebrados);
    printf("\n");
    // Imprime as informações dos ciclistas
    for (int i = 0; i < g_num_ciclistas; i++) {
      Ciclista* ciclista = ciclistas[i];
      if (ciclista->na_corrida) {
        printf("Ciclista %d: x=%2d, y=%2d, voltas=%d, velocidade=%d\n",
              ciclista->id, ciclista->posicao_x, ciclista->posicao_y,
              ciclista->voltas, ciclista->velocidade);
      }
    }
    printf("\n");
    // Imprime os vencedores e quebrados
    for (int i = 0; i < g_num_ciclistas; i++) {
      if (vencedores[i] == 1) {
        printf("Ciclista %d ganhou na colocação %d.\n", i, ciclistas[i]->colocacao);
      } else if (quebrados[i] == 1) {
        printf("Ciclista %d quebrou na volta %d.\n", i, ciclistas[i]->voltas);
      }
    }
    // clear terminal screen
    // printf("\033[H\033[J");
    printf("-----------------------------------------------------------------\n");
  }

  return;
}

void imprime_pista() {
  pthread_mutex_lock(&mutex_pista);
  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < g_tamanho_pista; i++) {
      if (pista[i][j] == -1) {
        printf("  .");
      } else {
        printf("%3d", pista[i][j]);
      }
      // printf("%3d", pista[i][j]);
    }
    printf("\n");
  }
  pthread_mutex_unlock(&mutex_pista);
}

int main(int argc, char *argv[]) {
  int modo = -1;
  Ciclista* ciclista;
  bool sai_do_loop = false;
  srand(time(NULL));

  // Manipula a entrada
  if (argc == 3) {
    modo = 0; // Modo padrão
  } else if (argc == 4) {
    modo = 1; // Modo debug
  } else {
    printf("Uso: %s <d> <k> [-debug]\n", argv[0]);
    return 1;
  }

  // Inicializa as variáveis globais
  g_tamanho_pista = atoi(argv[1]);
  g_num_ciclistas = g_num_na_corrida = atoi(argv[2]);
  g_num_vencedores = g_num_quebrados = 0;
  g_num_voltas_completas = 2;
  cria_mutexes_barreiras();
  cria_ciclistas();
  cria_pista();
  cria_largada();
  cria_threads();

  // Bitmaps para verificar os vencedores e quebrados de cada volta;
  int* vencedores = (int*) malloc(g_num_ciclistas * sizeof(int));
  int* quebrados = (int*) malloc(g_num_ciclistas *sizeof(int));

  // Loop principal
  while (!sai_do_loop) {
    // printf("m: to no loop\n");
    // Sincroniza os ciclistas que já andaram
    // printf("m: Esperando na barreira 1\n");
    pthread_barrier_wait(&barreira_ciclistas_andaram);
    // printf("m: Passei da barreira 1\n");

    // Avança um tick do clock (60ms)
    // usleep(60000);
    usleep(90000);

    // Verifica se alguém ganhou ou quebrou
    for (int i = 0; i < g_num_ciclistas; i++) {
      vencedores[i] = 0; quebrados[i] = 0;
    }
    for (int i = 0; i < g_num_ciclistas; i++) {
      ciclista = ciclistas[i];
      if (ciclista->na_corrida) {
        if (ciclista->ganhou) {
          g_num_vencedores++;
          ciclista->colocacao = g_num_vencedores;
          ciclista->na_corrida = false;
          vencedores[i] = 1;
          g_num_na_corrida--;
          // Atualiza o número de voltas da corrida
        } else if (ciclista->quebrou) {
          quebrados[i] = 1;
          ciclista->na_corrida = false;
          g_num_na_corrida--;
        }
      }
    }
    for (int i = 0; i < g_num_ciclistas; i++) {
      if (vencedores[i] == 1) {
        pthread_mutex_lock(&mutex_voltas);
        g_num_voltas_completas = g_num_voltas_completas + 2;
        pthread_mutex_unlock(&mutex_voltas);
        break;
      }
    }


    // Processa os dados dos vencedores e quebrados
    // e imprime as informações na tela
    imprime_corrida(modo, vencedores, quebrados);

    // Reseta os dados dos vencedores e quebrados
    for (int i = 0; i < g_num_ciclistas; i++) {
      vencedores[i] = 0;
      quebrados[i] = 0;
    }

    if (g_num_na_corrida == 0) {
      printf("Todos os ciclistas terminaram a corrida\n");
      sai_do_loop = true;
    }

    // Os ciclistas aguardam a main imprimir a corrida antes de prosseguirem
    // printf("m: Esperando na barreira 2\n");
    pthread_barrier_wait(&barreira_impressao);
    // printf("m: Passei da barreira 2\n");
  }

  // Libera a memória
  destroi_threads();
  destroi_mutexes_barreiras();
  destroi_ciclistas();
  destroi_pista();

  free(vencedores);
  free(quebrados);

  return 0;
}