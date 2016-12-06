#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdbool.h>

#define ALPHA_SIZE 256
#define Z ALPHA_SIZE * 2 - 1

// define variable types
typedef struct block {
  int weight;    // weight of nodes in block
  int parent;    // parent of block leader
  int parity;    // 1 if block leader a left branch or root, othw 0
  int rtChild;   // right child of leader
  int first;     // first node of block
  int last;      // last node of block   
  int prevBlock; // circular linked list   
  int nextBlock;
} block_t;

typedef struct node {
  int block;     // block this node belongs to
  int alpha;     // alphabet letter this node corresponds to
} node_t;

/* Initialise Huffman tree as per Vitter 1989, variable names equate to
 * those in original paper.
 *
 * M - number of zero-weight letters in alphabet
 * M = 2^E + R
 * n - the alphabet size
 * nodes - array of node_type
 * rep - array of node correspondences to letter
 * blocks - array of block_type
 * availBlock - first block available if list is nonempty, 0 otherwise
 */
void initialise(int *M, int *E, int *R, node_t *nodes, int *rep, 
  block_t *blocks, int *availBlock);

void update(int k, int *M, int *E, int *R, node_t *nodes, int *rep,
  block_t *blocks, int *availBlock);

int findNode(int k, int *M, int *E, int *R, node_t *nodes, int *rep,
    block_t *blocks, int *availBlock, int *leafToIncrement);

void interchangeLeaves(int e1, int e2, int *rep, node_t *nodes);

void slideAndIncrement(int *q, node_t *nodes, block_t *blocks, int *availBlock);

int findChild(int j, int parity, node_t *nodes, block_t *blocks);

#endif
