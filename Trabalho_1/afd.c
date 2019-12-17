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

int isSymbolValid(char symbol, afd_t afd) {
  for(int i=0; i<afd.symbols_quantity; i++){
    if(afd.alphabet[i] == symbol){
      return i;
    }
  }
  return -1;
}

int isFinalState(int state, afd_t afd) {
  for(int i=0; i<afd.final_state_quantity; i++){
    if(afd.F[i] == state)
      return afd.F[i];
  }
  return -1;
}

int hasTransition(int state, int symbol, afd_t afd) {
  return afd.transitions[state][symbol];
}

int countStates(char *states) {
  int count = 0;
  while(*states != '\0'){
    if(*states == ' ')
      ++count;
    ++states;
  }
  return ++count;
}

void removeBlankCharacters(char* s) {
  const char* d = s;
  do {
    while (*d == ' ')
      ++d;
  } while (*s++ = *d++);
}

void readLine(FILE* fp, char* line) {
  fgets(line, BUFFER_SIZE, fp);
}

void readAlphabet(afd_t* afd, char* alphabet) {
  strcpy(afd->alphabet, alphabet);
}

void readStates(int* state_section, char* states) {
  char* token = strtok(states, " ");
  int i = 0;

  while(token != NULL) {
    state_section[i] = strtol(token, (char **)NULL, 10);
    token = strtok(NULL, " ");
    ++i;
  }
}

void readTransitions(char* description, int* state, char* symbol, int* transition) {
  char* token = strtok(description, " ");
  *state = (int)strtol(token, (char **)NULL, 10);
  token = strtok(NULL, " ");
  *symbol = *token;
  token = strtok(NULL, " ");
  *transition = (int)strtol(token, (char **)NULL, 10);
}

void readWord(char* word) {
  printf("Insira a palavra a ser testada (Digite -1 para sair): ");
  scanf("%[^\n]s", word);
  getchar();
}

int main(int argc, char *argv[]) {
  int i, j;
  char line[BUFFER_SIZE], word[BUFFER_SIZE]; 
  afd_t *afd = malloc(sizeof *afd);
  FILE *fp;

  if(argc >= 2) {
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
    if(i == 0){
      removeBlankCharacters(line);
      afd->symbols_quantity = strlen(line);
      afd->alphabet = malloc(sizeof *afd->alphabet * afd->symbols_quantity);
      readAlphabet(afd, line);
    } else if(i == 1) {
      afd->states_quantity = countStates(line);
      afd->E = malloc(sizeof *afd->E * afd->states_quantity);
      readStates(afd->E, line);
    } else if(i == 2) {
      afd->final_state_quantity = countStates(line);
      afd->F = malloc(sizeof *afd->F * afd->final_state_quantity);
      readStates(afd->F, line);
    } else if(i == 3) {
      afd->i = strtol(line, (char **)NULL, 10);
    }
  }

  // Aloca a matriz dinamicamente
  afd->transitions = malloc(sizeof *afd->transitions * afd->states_quantity);
  for(i = 0; i < afd->states_quantity; i++) {
    afd->transitions[i] = malloc(sizeof **afd->transitions * afd->symbols_quantity);
    for(j=0; j<afd->symbols_quantity; j++) {
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
    afd->transitions[state][symbol_position] = transition;
  }

  readWord(word);
  while(strcmp(word, "-1") != 0) {
    afd->current_state = afd->i;
    for(i=0; i<strlen(word); i++) {
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