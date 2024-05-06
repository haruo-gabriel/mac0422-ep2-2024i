#include "ep2.h"

void cria_mutex_pista(int tamanho_pista) {
  mutex_pista = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * tamanho_pista);
  return;
}

void destroi_mutex_pista(int tamanho_pista) {
  for (int i = 0; i < tamanho_pista; i++) {
    pthread_mutex_destroy(&mutex_pista[i]);
  }
  free(mutex_pista);
  return;
}

// void cria_mutex_ciclistas(int num_ciclistas) {
//   mutex_ciclistas = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * num_ciclistas);
//   for (int i = 0; i < num_ciclistas; i++) {
//     pthread_mutex_init(&mutex_ciclistas[i], NULL);
//   }
// }

// void destroi_mutex_ciclistas(int num_ciclistas) {
//   for (int i = 0; i < num_ciclistas; i++) {
//     pthread_mutex_destroy(&mutex_ciclistas[i]);
//   }
//   free(mutex_ciclistas);
// }

void cria_pista(int tamanho_pista) {
  // Aloca memória para a pista
  pista = (int**) malloc(tamanho_pista * sizeof(int*));
  for (int i = 0; i < 10; i++) {
    pista[i] = (int*) malloc(10 * sizeof(int));
  }
  if (pista == NULL) {
    perror("Erro ao alocar memória para a pista");
    return;
  }

  // Inicializa a matriz da pista
  for (int i = 0; i < tamanho_pista; i++) {
    for (int j = 0; j < 10; j++) {
      pista[i][j] = -1;
    }
  }
}

void destroi_pista(int tamanho_pista) {
  for (int i = 0; i < tamanho_pista; i++) {
    free(pista[i]);
  }
  free(pista);
}

void cria_ciclistas(int num_ciclistas) {
  // Aloca memória para os ciclistas
  ciclistas = (Ciclista**) malloc(num_ciclistas * sizeof(Ciclista*));
  for (int i = 0; i < num_ciclistas; i++) {
    ciclistas[i] = cria_ciclista(i);
    if (ciclistas[i] == NULL) {
      perror("Erro ao alocar memória para um ciclista");
      return;
    }
  }
  if (ciclistas == NULL) {
    perror("Erro ao alocar memória para os ciclistas");
    return;
  }
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
  ciclista->atualizou_posicao = false;
  ciclista->atualizou_velocidade = false;

  return ciclista;
}

void destroi_ciclistas(int num_ciclistas) {
  for (int i = 0; i < num_ciclistas; i++) {
    free(ciclistas[i]);
  }
  free(ciclistas);
}

void cria_threads(int num_ciclistas) {
  threads = (pthread_t*) malloc(num_ciclistas * sizeof(pthread_t));
  if (threads == NULL) {
    perror("Erro ao alocar memória para as threads");
    return;
  }
  for (int i = 0; i < num_ciclistas; i++) {
    ThreadArgs* arg = (ThreadArgs*) malloc(sizeof(ThreadArgs));
    arg->id = i;
    pthread_create(&threads[i], NULL, f_ciclista, (void*)arg);
  }
}

void cria_largada(int num_ciclistas) {
  // Cria um vetor para aleatorizar a ordem de largada
  int* array = malloc(num_ciclistas * sizeof(int));
  for(int i = 1; i < num_ciclistas+1; i++) {
    array[i] = i;
  }
  for (int i = num_ciclistas-1; i > 0; i--) {
    int j = rand() % (i+1);
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }

  int colunas = (num_ciclistas + 4) / 5; // quantas colunas de ciclistas teremos
  int i = 0; // contador para o array de ciclistas
  for (int x=colunas-1; x>=0; x--) {
    for (int y=0; y<5; y++) {
      ciclistas[array[i-1]]->posicao_x = x;
      ciclistas[array[i-1]]->posicao_x = y;
    }
    i++;
  }
}

void* f_ciclista(void* arg) {
  int id = ((ThreadArgs*)arg)->id;
  free(arg);
  Ciclista* ciclista = ciclistas[id];
  int x = ciclista->posicao_x;
  int y = ciclista->posicao_y;
  
  // A velocidade para todos na primeira volta é 30 km/h, logo
  // não é necessário atualizá-la
  while (ciclista->voltas == 0) {
    atualiza_posicao(id, x, y);

    pthread_barrier_wait(&barreira_1);
    pthread_barrier_wait(&barreira_2);
  }
  // A partir da segunda volta, a velocidade dos ciclistas podem mudar
  while (true) {
    // Se o ciclista terminou uma volta, atualiza sua velocidade
    if (x == 0) {
      atualiza_velocidade(id);
    }

    // Atualiza a posição do ciclista na pista
    atualiza_posicao(id, x, y);

    // Verifica se o ciclista terminou a corrida
    if (ciclista->voltas % g_num_voltas_completas == 0) {
      ciclista->colocacao = ciclista->voltas / 2;
      break;
    }

    // Sorteia a quebra do ciclista
    if (ciclista->voltas % 6 == 0 && quebra_ciclista(id)) {
      break;
    }

    pthread_barrier_wait(&barreira_1);
    pthread_barrier_wait(&barreira_2);
  }

  pthread_exit(NULL);
}

void atualiza_posicao(int id, int x, int y) {
  pthread_mutex_lock(&mutex_pista[(x+1)%g_tamanho_pista]);
  pthread_mutex_lock(&mutex_pista[x]);

  Ciclista* ciclista = ciclistas[id];
  
  if (ciclista->velocidade == 60 ||
     (ciclista->velocidade == 30 && !ciclista->atualizou_posicao))
  {
    // Se há um ciclista à frente
    if (!avanca_pra_frente(id, x, y)) {
      if (y == 0) { // Se o ciclista está na borda inferior
        ultrapassa_por_cima(id, x, y);
      } else if (y == 9) { // Se o ciclista está na borda superior
        ultrapassa_por_baixo(id, x, y);
      } else { // Se o ciclista está no meio da pista
        // Sorteia se o ciclista vai tentar ultrapassar primeiro por cima ou por baixo
        if (rand() % 2 == 0) {
          if (!ultrapassa_por_cima(id, x, y))
            ultrapassa_por_baixo(id, x, y);
        } else {
          if (!ultrapassa_por_baixo(id, x, y))
            ultrapassa_por_cima(id, x, y);
        }
      }
    }
    ciclista->atualizou_posicao = true;
  } else {
    ciclista->atualizou_posicao = false;
  }

  pthread_mutex_unlock(&mutex_pista[x]);
  pthread_mutex_unlock(&mutex_pista[(x+1)%g_tamanho_pista]);

  return;
}

bool avanca_pra_frente(int id, int x, int y) {
  Ciclista* ciclista = ciclistas[id];

  if (pista[x][y+1] == -1) {
    // Ciclista deixa a posição atual livre
    pista[x][y] = -1;
    // Ciclista avança e ocupa a posição à frente
    pista[x][y+1] = ciclista->id;
    return true;
  }

  return false;
}

bool ultrapassa_por_cima(int id, int x, int y) {
  Ciclista* ciclista = ciclistas[id];

  if (pista[(x+1)%g_tamanho_pista][y+1] == -1) {
    // Ciclista deixa a posição atual livre
    pista[x][y] = -1;
    // Ciclista ultrapassa e ocupa a posição à diagonal
    pista[(x+1)%g_tamanho_pista][y+1] = ciclista->id;
    return true;
  }

  return false;
}

bool ultrapassa_por_baixo(int id, int x, int y) {
  Ciclista* ciclista = ciclistas[id];

  if (pista[(x+1)%g_tamanho_pista][y-1] == -1) {
    // Ciclista deixa a posição atual livre
    pista[x][y] = -1;
    // Ciclista ultrapassa e ocupa a posição à diagonal
    pista[(x+1)%g_tamanho_pista][y-1] = ciclista->id;
    return true;
  }
  return false;
}

void atualiza_velocidade(int id) {
  pthread_mutex_lock(&mutex_ciclistas);

  Ciclista* ciclista = ciclistas[id];

  // Sorteia a velocidade do ciclista
  if (ciclista->velocidade == 30) {
    // 70% de chance de aumentar a velocidade para 70 km/h
    if (rand() % 100 < 30) {
      ciclista->velocidade = 30;
    } else {
      ciclista->velocidade = 60;
    } 
  } else if (ciclista->velocidade == 60) {
    // 50% de chance de aumentar a velocidade para 70 km/h
    if (rand() % 100 < 50) {
      ciclista->velocidade = 30;
    } else {
      ciclista->velocidade = 60;
    }
  }

  // Atualiza a velocidade de todos os ciclistas que estão imediatamente atrás
  // e que não podem ultrapassar
  if (ciclista->velocidade == 30) {
    int x = (ciclista->posicao_x - 1) % g_tamanho_pista;
    int y = ciclista->posicao_y;
    while (pista[x][y] != -1) {
    }
  }

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
    // Imprime a pista
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 10; j++) {
        if (pista[i][j] == -1) {
          printf(".  ");
        } else {
          printf("%d ", pista[i][j]);
        }
      }
      printf("\n");

    }
  }
}

int main(int argc, char *argv[]) {
  int modo = -1;
  int tamanho_pista = -1;
  int num_ciclistas = -1;
  int num_vencedores = 0;
  int num_na_corrida;
  pista = NULL;
  ciclistas = NULL;
  threads = NULL;
  srand(time(NULL));
  // Bitmaps para verificar os vencedores e quebrados de cada volta;
  int* vencedores = (int*) malloc(num_ciclistas * sizeof(int));
  int* quebrados = (int*) malloc(num_ciclistas *sizeof(int));

  // Manipula a entrada
  if (argc == 3) {
    modo = 0; // Modo padrão
  } else if (argc == 4) {
    modo = 1; // Modo debug
  } else {
    printf("Uso: %s <d> <k> [-debug] \n", argv[0]);
    return 1;
  }

  // Inicializa as variáveis globais
  g_num_ciclistas = atoi(argv[2]);
  num_ciclistas = num_na_corrida = atoi(argv[2]);
  g_tamanho_pista = atoi(argv[3]);
  tamanho_pista = atoi(argv[3]);
  cria_pista(tamanho_pista);
  cria_ciclistas(num_ciclistas);
  cria_largada(num_ciclistas);
  cria_threads(num_ciclistas);
  // pthread_mutex_init(&mutex_pista, NULL);
  cria_mutex_pista(tamanho_pista);
  pthread_mutex_init(&mutex_ciclistas, NULL);
  // cria_mutex_ciclistas(num_ciclistas);
  pthread_barrier_init(&barreira_1, NULL, g_num_ciclistas+1);
  pthread_barrier_init(&barreira_2, NULL, g_num_ciclistas+1);

  clock_ms = 0;
  Ciclista* ciclista;

  // Loop principal
  while (num_vencedores <= num_na_corrida - 2) {
    // A main aguarda as threads fazerem todas as operações
    pthread_barrier_wait(&barreira_1);

    // Verifica se alguém ganhou
    for (int i = 0; i < num_ciclistas; i++) {
      vencedores[i] = false;
      quebrados[i] = false;
    }
    for (int i = 0; i < num_ciclistas; i++) {
      ciclista = ciclistas[i];
      if (ciclista->na_corrida) {
        if (ciclista->ganhou) {
          num_vencedores++;
          ciclista->colocacao = num_vencedores;
          vencedores[i] = true;
        } else if (ciclista->quebrou) {
          quebrados[i] = true;
        }
        num_na_corrida--;
      }
    }
    g_num_voltas_completas = g_num_voltas_completas + 2;

    // Processa os dados dos vencedores e quebrados
    // e imprime as informações na tela
    imprime_corrida(modo, vencedores, quebrados);

    for (int i = 0; i < num_ciclistas; i++) {
      vencedores[i] = false;
      quebrados[i] = false;
    }

    // As threads aguardam a main imprimir os dados
    pthread_barrier_wait(&barreira_2);

    // Avança um tick do clock (60ms)
    usleep(60000);

  }

  // Gera a saída

  // Libera a memória
  destroi_pista(tamanho_pista);
  destroi_ciclistas(num_ciclistas);
  destroi_mutex_pista(tamanho_pista);
  // destroi_mutex_ciclistas(num_ciclistas);
  free(threads);
  free(vencedores);

  return 0;
}