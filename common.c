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

void update(int k, int *M, int *E, int *R, int n, node_t *nodes, int *rep,
  block_t *blocks, int *availBlock) {
  int leafToIncrement = 0;
  int q = findNode(k, M, E, R, n, nodes, rep, blocks, availBlock, 
    &leafToIncrement);
  while (q > 0) {
    slideAndIncrement();
  }
  if (leafToIncrement != 0) {
    q = leafToIncrement;
    slideAndIncrement();
  }
}

int findNode(int k, int *M, int *E, int *R, int n, node_t *nodes, int *rep,
    block_t *blocks, int *availBlock, int *leafToIncrement) {
  int q = rep[k];
  *leafToIncrement = 0;
  if (q <= *M) {
    interchangeLeaves(q, *M);
    if (*R == 0) {
      *R = *M / 2;
      if (*R > 0) {
        *E = *E - 1;
      }
    }
    *M = *M - 1;
    *R = *R - 1;
    q = *M + 1;
    int bq = nodes[q].block;
    if (*M > 0) {
      // split the 0-node into an internal node with two children. The new
      // 0-node is node M; the old 0-node is node M + 1; the new parent of
      // nodes M and M + 1 is node M + n
      nodes[*M].block = bq;
      blocks[bq].last = *M;
      int oldParent = blocks[bq].parent;
      blocks[bq].parent = *M + n;
      blocks[bq].parity = 1;
      // Create a new internal block of zero weight for node M + n
      int b = *availBlock;
      *availBlock = blocks[*availBlock].nextBlock;
      blocks[b].prevBlock = bq;
      blocks[b].nextBlock = blocks[bq].nextBlock;
      blocks[blocks[bq].nextBlock].prevBlock = b;
      blocks[bq].nextBlock = b;
      blocks[b].parent = oldParent;
      blocks[b].parity = 0;
      blocks[b].rtChild = q;
      nodes[*M + n].block = b;
      blocks[b].weight = 0;
      blocks[b].first = *M + n;
      blocks[b].last = *M + n;
      *leafToIncrement = q;
      q = *M + n;
    }
  }
  else {
    // interchange q with the first node in q's block
    interchangeLeaves(q, blocks[nodes[q].block].first);
    q = blocks[nodes[q].block].first;
    if ((q == *M + 1) && (*M > 0)) {
      *leafToIncrement = q;
      q = blocks[nodes[q].block].parent;
    }
  }
  return q;
}

void interchangeLeaves(int e1, int e2) {
  printf("should be interchanging %d and %d\n", e1, e2);
}

void slideAndIncrement() {
  printf("should be slide and incrementing\n");
}
