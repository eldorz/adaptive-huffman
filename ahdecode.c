#define __STDC_WANT_LIB_EXT2__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "common.h"

int receive(char *message, size_t *pos) {
  if (message[*pos] == 0) {
    fprintf(stderr, "receive: unexpected end of string\n");
    exit(1);
  }
  int retval = message[*pos] - (int)'0';
  if (retval != 0 && retval != 1) {
    fprintf(stderr, "receive: unexpected input: %c\n", message[*pos]);
    exit(1);
  }
  ++(*pos);
  return retval;
}

int receiveAndDecode(char *message, size_t *pos, int *M, int *E, int *R,
  node_t *nodes, block_t *blocks) {
  int q = 0;
  if (*M == ALPHA_SIZE) q = ALPHA_SIZE;
  else q = Z;
  while (q > ALPHA_SIZE) { // traverse down the tree
    q = findChild(q, receive(message, pos), nodes, blocks);
  }
  if (q == *M) { // decode 0 node
    q = 0;
    for (int i = 1; i <= *E; ++i) {
      q = 2 * q + receive(message, pos);
    }
    if (q < *R) q = 2 * q + receive(message, pos);
    else q = q + *R;
    q = q + 1;
  }
  return nodes[q].alpha;
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
  size_t pos = 0;
  unsigned char buf[MAX_PLAINTEXT_SIZE];
  int i = 0;
  while (pos < len - 1) {
    // bytecode alphabet is 0 to 255, so the jth 'letter' is bytecode + 1
    int j = receiveAndDecode(message, &pos, &M, &E, &R, nodes, blocks);
    // add aj to message buffer
    buf[i++] = (unsigned char)j - 1;
    if (sflag == 1 && message[pos] == ' ') ++pos;
    update(j, &M, &E, &R, nodes, rep, blocks, &availBlock);
  }
  buf[i] = 0;  // null terminate the string
  printf("%s\n", buf);
  
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
