#define __STDC_WANT_LIB_EXT2__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "common.h"

void encodeAndTransmit(int j, int *M, int *R, int *E, block_t *blocks,
  node_t *nodes, int *rep) {
  int q = rep[j];  // get a node for letter j
  int i = 0;
  int t = 0;
  int stack[ALPHA_SIZE];
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
  if (*M == ALPHA_SIZE) root = ALPHA_SIZE;
  else root = 2 * ALPHA_SIZE - 1;
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

void ahencode(unsigned char *message, int len, int sflag) {
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
    int j = message[i] + 1;
    
    /* debug
    printf("-----------------------------------------\n");
    printf("State prior to update with character '%c', j of %d\n", message[i], j);
    for (int k = 251; k <= 256; ++k) {
      printf("nodes[%d] = {block = %d, alpha = %d}\n", k, nodes[k].block,
        nodes[k].alpha);
    }
    for (int k = 507; k <= 511; ++k) {
      printf("nodes[%d] = {block = %d, alpha = %d}\n", k, nodes[k].block,
        nodes[k].alpha);
    }
    for (int k = 1; k <= 8; ++k) {
      printf("blocks[%d] = {weight = %d, parent = %d, parity = %d, rtChild = %d, first = %d, last = %d, prevBlock = %d, nextBlock = %d}\n", k, blocks[k].weight, blocks[k].parent, blocks[k].parity, blocks[k].rtChild, blocks[k].first, blocks[k].last, blocks[k].prevBlock, blocks[k].nextBlock);
    } */


    encodeAndTransmit(j, &M, &R, &E, blocks, nodes, rep);
    update(j, &M, &E, &R, nodes, rep, blocks, &availBlock);
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
  int i = 0;
  while ((byteCount = getline(&nextline, &size, stdin)) != -1) {
    ahencode((unsigned char *)nextline, byteCount, sflag);
  }

  // free heap variables
  free(nextline);
  return 0;
}
