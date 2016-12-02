#define __STDC_WANT_LIB_EXT2__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "common.h"

// constants
#define ALPHABET_SIZE 8

void char_to_binstring(char c, char *binstring) {
  int next = 0;
  for (int i = 7; i >= 0; --i) {
    next = c & 00000001;
    binstring[i] = 48 + next; // 48 is ASCII '0'
    c = c >> 1;
  }
  binstring[8] = 0;             // null terminate
}


int main(int argc, char **argv) {
  // process arguments
  int sflag = 0;
  if (argc == 2 && strcmp(argv[1], "-s") == 0) sflag = 1;
  else if (argc != 1) {
    fprintf(stderr, "Usage: ahencode [-s]\n");
    return 1;
  }

  // stack variables
  int M = 0;
  int R = 0;
  int E = 0;
  int Z = 2 * ALPHABET_SIZE - 1;

  // heap variables
  block_t *blocks = malloc((Z + 1) * sizeof(block_t));
  node_t *nodes = malloc((Z + 1) * sizeof(node_t));
  int *rep = malloc(ALPHABET_SIZE * sizeof(int));  // node representing letter

  // initialise
  int availBlock = 0;
  initialise(&M, &E, &R, ALPHABET_SIZE, nodes, rep, blocks, &availBlock);
  
  printf("M = %d, E = %d, R = %d\n", M, E, R);
  for (int i = 1; i <= Z; ++i) {
    printf("node %d: block: %d alpha: %d\n", i, nodes[i].block, nodes[i].alpha);
  }
  for (int i = 1; i <= Z; ++i) {
    printf("block %d: weight: %d parent: %d parity: %d rtChild: %d first: %d last: %d prevBlock: %d nextBlock: %d\n", i, blocks[i].weight, blocks[i].parent, 
      blocks[i].parity, blocks[i].rtChild, blocks[i].first, blocks[i].last, 
      blocks[i].prevBlock, blocks[i].nextBlock);
  }
  
  // read lines from stdin, encode
  char *nextline = NULL;
  size_t size = 0;
  ssize_t byteCount;
  char binstring[9] = "";
  while ((byteCount = getline(&nextline, &size, stdin)) != -1) {
    for (int i = 0; i < byteCount - 1; ++i) {
      char nextchar = nextline[i];
      char_to_binstring(nextchar, binstring);
      printf("%s", binstring);
      if (sflag == 1 && i != byteCount - 2) printf(" ");
    }
  printf("\n");
  }

  // free heap variables
  free(nextline);
  free(blocks);
  free(nodes);
  free(rep);
  return 0;
}
