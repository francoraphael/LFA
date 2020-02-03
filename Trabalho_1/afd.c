#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 256

typedef struct {
  int** transitions;
  int* E;
  int* F;
  char* alphabet;
  int i;
  int states_quantity;
  int final_state_quantity;
  int symbols_quantity;
  int current_state;
} afd_t;

void isStateValid(int state, afd_t afd) {
  for (int i = 0; i < afd.states_quantity; i++) {
    if (afd.E[i] == state) return;
  }
  printf("Um estado nao definido foi encontrado... Saindo\n");
  exit(0);
}

// Verifica se o simbolo informado pertence ao alfabeto do AFD
int isSymbolValid(char symbol, afd_t afd) {
  for(int i = 0; i < afd.symbols_quantity; i++){
    if(afd.alphabet[i] == symbol){
      return i;
    }
  }
  return -1;
}

// Verifica se o estado informado é um estado final do AFD
int isFinalState(int state, afd_t afd) {
  for(int i = 0; i < afd.final_state_quantity; i++){
    if(afd.F[i] == state)
      return afd.F[i];
  }
  return -1;
}

// Informa a transicao entre um estado e simbolo no AFD
int hasTransition(int state, int symbol, afd_t afd) {
  return afd.transitions[state][symbol];
}

// Dado uma linha de estados no formato de entrada, verifica quantos
// estados existem na referida linha
int countStates(char *states) {
  int count = 0;
  while(*states != '\0'){
    if(*states == ' ')
      ++count;
    ++states;
  }
  return ++count;
}

// Remove os caracteres em branco de uma linha
void removeBlankCharacters(char* s) {
  const char* d = s;
  do {
    while (*d == ' ')
      ++d;
  } while (*s++ = *d++);
}

// Lê uma linha de um arquivo
void readLine(FILE* fp, char* line) {
  fgets(line, BUFFER_SIZE, fp);
}

// Lê o alfabeto de uma linha para o AFD
void readAlphabet(char* alphabet, afd_t* afd) {
  strcpy(afd->alphabet, alphabet);
}

// Lê os estados de uma linha para um dos conjuntos de estado
void readStates(int* state_section, char* states, int is_reading_initial_state, afd_t afd) {
  char* token = strtok(states, " ");
  int i = 0, state;

  while(token != NULL) {
    state = strtol(token, (char **)NULL, 10);
    if (!is_reading_initial_state) {
      isStateValid(state, afd); 
    }
    state_section[i] = state;
    token = strtok(NULL, " ");
    ++i;
  }
}

// Lê uma transição e armazena nas variáveis de retorno
void readTransitions(char* description, int* state, char* symbol, int* transition) {
  char* token = strtok(description, " ");
  *state = (int)strtol(token, (char **)NULL, 10);
  token = strtok(NULL, " ");
  *symbol = *token;
  token = strtok(NULL, " ");
  *transition = (int)strtol(token, (char **)NULL, 10);
}

// Lê uma nova palavra a ser validada
void readWord(char* word) {
  printf("Insira a palavra a ser testada (Digite -1 para sair): ");
  scanf("%[^\n]s", word);
  getchar();
}

int main(int argc, char *argv[]) {
  int i, j, auxiliar_state;
  char line[BUFFER_SIZE], word[BUFFER_SIZE]; 
  afd_t *afd = malloc(sizeof *afd);
  FILE *fp;

  // Verificações para abrir e ler arquivo
  if(argc == 2) {
    if((fp = fopen(argv[1], "r")) == NULL){
      printf("Falha ao abrir arquivo\n");
      return 0;
    }
  } else {
    printf("Arquivo nao informado\n");
    return 0;
  }

  // Lê as quatro primeiras linhas do arquivo
  // Alfabeto, Estados, Estados Finais e Estado Inicial
  for(i = 0; i < 4; i++) {
    readLine(fp, line);
    if(i == 0) {
      removeBlankCharacters(line);
      afd->symbols_quantity = strlen(line);
      afd->alphabet = malloc(sizeof *afd->alphabet * afd->symbols_quantity);
      readAlphabet(line, afd);
    } else if(i == 1) {
      afd->states_quantity = countStates(line);
      afd->E = malloc(sizeof *afd->E * afd->states_quantity);
      readStates(afd->E, line, 1, *afd);
    } else if(i == 2) {
      afd->final_state_quantity = countStates(line);
      afd->F = malloc(sizeof *afd->F * afd->final_state_quantity);
      readStates(afd->F, line, 0, *afd);
    } else if(i == 3) {
      auxiliar_state = strtol(line, (char **)NULL, 10);
      isStateValid(auxiliar_state, *afd);
      afd->i = auxiliar_state;
    }
  }

  // Aloca a matriz dinamicamente
  afd->transitions = malloc(sizeof *afd->transitions * afd->states_quantity);
  for(i = 0; i < afd->states_quantity; i++) {
    afd->transitions[i] = malloc(sizeof **afd->transitions * afd->symbols_quantity);
    for(j=0; j < afd->symbols_quantity; j++) {
      afd->transitions[i][j] = -1;
    }
  }

  // Lê as transições e armazena na matriz
  while(fgets(line, BUFFER_SIZE, fp) != NULL) {
    char symbol;
    int state, transition, symbol_position;
    readTransitions(line, &state, &symbol, &transition);
    if((symbol_position = isSymbolValid(symbol, *afd)) == -1) {
      printf("Simbolo '%c' nao definido no alfabeto\n", symbol);
      return 0;
    }
    isStateValid(state, *afd);
    afd->transitions[state][symbol_position] = transition;
  }

  readWord(word);
  while(strcmp(word, "-1") != 0) {
    afd->current_state = afd->i;
    for(i = 0; i < strlen(word); i++) {
      int symbol_position, target_state;
      if((symbol_position = isSymbolValid(word[i], *afd)) != -1) {
        if((target_state = hasTransition(afd->current_state, symbol_position, *afd)) != -1){
          afd->current_state = target_state;
          continue;
        }
      }
      printf("Palavra '%s' nao reconhecida pela linguagem\n", word);
      goto read_new;
    }

    if(i == strlen(word) && isFinalState(afd->current_state, *afd) != -1)
      printf("Palavra '%s' reconhecida pela linguagem\n", word);
    else printf("Palava '%s' nao reconhecida pela linguagem\n", word);
    
    read_new: readWord(word);
  }
  return 0;
}