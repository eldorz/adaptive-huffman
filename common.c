#include "common.h"

void initialise(int *M, int *E, int *R, int n, node_t *nodes, int *rep, 
  block_t *blocks, int *availBlock) {
  *M = 0;
  *E = 0;
  *R = -1;
  int Z = 2 * n - 1;
  for (int i = 1; i <= n; ++i) {
    // calculate exponent and residual for zero-weight nodes
    *M = *M + 1; *R = *R + 1;
    if (*R * 2 == *M) {
      *E = *E + 1;
      *R = 0;
    }
    // assign alphabet to nodes and nodes to alphabet
    nodes[i].alpha = i;
    rep[i] = i;
  }

  // initialise node n as the 0-node
  nodes[n].block = 1;      // node n is block 1
  blocks[1].prevBlock = 1; // circular list with only one element
  blocks[1].nextBlock = 1;
  blocks[1].weight = 0;    // no letters seen yet
  blocks[1].first = n;     // node n is the first and last node of block 1
  blocks[1].last = n;
  blocks[1].parity = 0;    // the lead of block 1 is the root of the tree
  blocks[1].parent = 0;    // the lead of block 1 has no parent

  // initialise available block list
  *availBlock = 2;  // next available block number is 2
  for (int i = *availBlock; i <= Z - 1; ++i) {
    blocks[i].nextBlock = i + 1;
  }
  blocks[Z].nextBlock = 0;
}
