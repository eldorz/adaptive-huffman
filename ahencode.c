#define __STDC_WANT_LIB_EXT2__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "common.h"

// constants
#define ALPHABET_SIZE 256

void char_to_binstring(char c, char *binstring) {
  int next = 0;
  for (int i = 7; i >= 0; --i) {
    next = c & 00000001;
    binstring[i] = 48 + next; // 48 is ASCII '0'
    c = c >> 1;
  }
  binstring[8] = 0;             // null terminate
}

void encodeAndTransmit(int j, int *M, int *R, int *E, int n, block_t *blocks,
  node_t *nodes, int *rep) {
  int q = rep[j];  // get a node for letter j
  int i = 0;
  int t = 0;
  int stack[9];
  int root = 0;
  if (q <= *M) {   // encode letter of 0 weight
    q = q - 1;
    if (q < 2 * *R) t = *E + 1;
    else {
      q = q - *R;
      t = *E;
    }
    for (int ii = 1; ii <= t; ++ii) {
      ++i;
      stack[i] = q % 2;
      q = q / 2;
    }
    q = *M;
  }
  if (*M == n) root = n;
  else root = 2 * n - 1;
  while (q != root) {  // traverse up the tree
    ++i;
    int qBlock = nodes[q].block;
    stack[i] = (blocks[qBlock].first - q + blocks[qBlock].parity) % 2;
    q = blocks[qBlock].parent - (blocks[qBlock].first - q + 1 - 
      blocks[qBlock].parity) / 2;
  }
  for (int ii = i; ii >= 1; --ii) {
    printf("%d", stack[ii]);
  }
}

void ahencode(char *message, int len, int sflag) {
  // stack variables
  int M = 0;
  int R = 0;
  int E = 0;
  int Z = 2 * ALPHABET_SIZE - 1;

  // heap variables
  block_t *blocks = malloc((Z + 1) * sizeof(block_t));
  node_t *nodes = malloc((Z + 1) * sizeof(node_t));
  int *rep = malloc((ALPHABET_SIZE + 1) * sizeof(int));  // node representing letter

  // initialise
  int availBlock = 0;
  initialise(&M, &E, &R, ALPHABET_SIZE, nodes, rep, blocks, &availBlock);
 
  // debug
  /*
  printf("M = %d, E = %d, R = %d\n", M, E, R);
  for (int i = 1; i <= Z; ++i) {
    printf("node %d: block: %d alpha: %d\n", i, nodes[i].block, nodes[i].alpha);
  }
  for (int i = 1; i <= Z; ++i) {
    printf("block %d: weight: %d parent: %d parity: %d rtChild: %d first: %d last: %d prevBlock: %d nextBlock: %d\n", i, blocks[i].weight, blocks[i].parent, 
      blocks[i].parity, blocks[i].rtChild, blocks[i].first, blocks[i].last, 
      blocks[i].prevBlock, blocks[i].nextBlock);
  }
  */
  // end debug

  // main loop
  for (int i = 0; i < len - 1; ++i) {
    // bytecode alphabet is 0 to 255, so the jth 'letter' is bytecode + 1
    int j = message[i] + 1;
    encodeAndTransmit(j, &M, &R, &E, ALPHABET_SIZE, blocks, nodes, rep);
    update(j, &M, &E, &R, ALPHABET_SIZE, nodes, rep, blocks, &availBlock);
    if (sflag == 1) printf(" ");
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
  char binstring[9] = "";
  while ((byteCount = getline(&nextline, &size, stdin)) != -1) {
    ahencode(nextline, byteCount, sflag);
  }

  // free heap variables
  free(nextline);
  return 0;
}
