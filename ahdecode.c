#define __STDC_WANT_LIB_EXT2__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "common.h"

int receiveAndDecode() {
  return 0;
} 

void ahdecode(char *message, int len, int sflag) {
  // stack variables
  int M = 0;
  int R = 0;
  int E = 0;

  // heap variables
  block_t *blocks = malloc((Z + 1) * sizeof(block_t));
  node_t *nodes = malloc((Z + 1) * sizeof(node_t));
  int *rep = malloc((ALPHA_SIZE + 1) * sizeof(int));  // node representing letter

  // initialise
  int availBlock = 0;
  initialise(&M, &E, &R, nodes, rep, blocks, &availBlock);
 

  // main loop
  for (int i = 0; i < len - 1; ++i) {
    // bytecode alphabet is 0 to 255, so the jth 'letter' is bytecode + 1
    int j = receiveAndDecode();
    // add aj to message buffer
    printf("%d", j);
    update(j, &M, &E, &R, nodes, rep, blocks, &availBlock);
  }
  printf("\n");
  
  // free heap variables
  free(blocks);
  free(nodes);
  free(rep);
}

int main(int argc, char **argv) {
  // process arguments
  int sflag = 0;
  if (argc == 2 && strcmp(argv[1], "-s") == 0) sflag = 1;
  else if (argc != 1) {
    fprintf(stderr, "Usage: ahencode [-s]\n");
    return 1;
  }

  // read lines from stdin, encode
  char *nextline = NULL;
  size_t size = 0;
  ssize_t byteCount;
  while ((byteCount = getline(&nextline, &size, stdin)) != -1) {
    ahdecode(nextline, byteCount, sflag);
  }

  // free heap variables
  free(nextline);
  return 0;
}
