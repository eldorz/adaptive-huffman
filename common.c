#include "common.h"

void initialise(int *M, int *E, int *R, node_t *nodes, int *rep, 
  block_t *blocks, int *availBlock) {
  *M = 0;
  *E = 0;
  *R = -1;
  for (int i = 1; i <= ALPHA_SIZE; ++i) {
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
  nodes[ALPHA_SIZE].block = 1;      // node n is block 1
  blocks[1].prevBlock = 1; // circular list with only one element
  blocks[1].nextBlock = 1;
  blocks[1].weight = 0;    // no letters seen yet
  blocks[1].first = ALPHA_SIZE;  // node n is the first and last node of block 1
  blocks[1].last = ALPHA_SIZE;
  blocks[1].parity = 0;    // the lead of block 1 is the root of the tree
  blocks[1].parent = 0;    // the lead of block 1 has no parent

  // initialise available block list
  *availBlock = 2;  // next available block number is 2
  for (int i = *availBlock; i <= Z - 1; ++i) {
    blocks[i].nextBlock = i + 1;
  }
  blocks[Z].nextBlock = 0;
}

void update(int k, int *M, int *E, int *R, node_t *nodes, int *rep,
  block_t *blocks, int *availBlock) {
  int leafToIncrement = 0;
  int q = findNode(k, M, E, R, nodes, rep, blocks, availBlock, 
    &leafToIncrement);
  while (q > 0) {
    slideAndIncrement(&q, nodes, blocks, availBlock);
  }
  if (leafToIncrement != 0) {
    q = leafToIncrement;
    slideAndIncrement(&q, nodes, blocks, availBlock);
  }
}

int findNode(int k, int *M, int *E, int *R, node_t *nodes, int *rep,
    block_t *blocks, int *availBlock, int *leafToIncrement) {
  int q = rep[k];
  *leafToIncrement = 0;
  if (q <= *M) {
    interchangeLeaves(q, *M, rep, nodes);
    if (*R == 0) {
      *R = *M / 2;
      if (*R > 0) *E = *E - 1;
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
      blocks[bq].parent = *M + ALPHA_SIZE;
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
      nodes[*M + ALPHA_SIZE].block = b;
      blocks[b].weight = 0;
      blocks[b].first = *M + ALPHA_SIZE;
      blocks[b].last = *M + ALPHA_SIZE;
      *leafToIncrement = q;
      q = *M + ALPHA_SIZE;
    }
  }
  else {
    // interchange q with the first node in q's block
    interchangeLeaves(q, blocks[nodes[q].block].first, rep, nodes);
    q = blocks[nodes[q].block].first;
    if ((q == *M + 1) && (*M > 0)) {
      *leafToIncrement = q;
      q = blocks[nodes[q].block].parent;
    }
  }
  return q;
}

void interchangeLeaves(int e1, int e2, int *rep, node_t *nodes) {
  rep[nodes[e1].alpha] = e2;
  rep[nodes[e2].alpha] = e1;
  int temp = nodes[e1].alpha;
  nodes[e1].alpha = nodes[e2].alpha;
  nodes[e2].alpha = temp;
}

void slideAndIncrement(int *q, node_t *nodes, block_t *blocks, 
  int *availBlock) {
  int bq_index = nodes[*q].block;
  block_t bq = blocks[bq_index];
  int nbq_index = bq.nextBlock;
  block_t nbq = blocks[nbq_index];
  int par = bq.parent;
  int oldParent = par;
  int oldParity = bq.parity;
  bool slide = false; 
  if (((*q <= ALPHA_SIZE) && (nbq.first > ALPHA_SIZE) && 
    (nbq.weight == bq.weight)) || ((*q > ALPHA_SIZE) && 
    (nbq.first <= ALPHA_SIZE) && 
    (nbq.weight == bq.weight + 1))) {
    // slide q over the next block
    bool slide = true;
    oldParent = nbq.parent;
    oldParity = nbq.parity;
    // adjust child pointers for next higher level in tree
    if (par > 0) {
      int bpar_index = nodes[par].block;
      block_t bpar = blocks[bpar_index];
      if (bpar.rtChild == *q) bpar.rtChild = nbq.last;
      else if (bpar.rtChild == nbq.first) bpar.rtChild = *q;
      else bpar.rtChild = bpar.rtChild + 1;
      if (par != Z) {
        int parPlusOneBlockIndex = nodes[par + 1].block;
	block_t parPlusOneBlock = blocks[parPlusOneBlockIndex];
        if (parPlusOneBlockIndex != bpar_index) {
	  if (parPlusOneBlock.rtChild == nbq.first)
	    parPlusOneBlock.rtChild = *q;
	  else if (nodes[parPlusOneBlock.rtChild].block == nbq_index)
	    parPlusOneBlock.rtChild = parPlusOneBlock.rtChild + 1;
	}
      }
      blocks[bpar_index] = bpar;
    }
    // adjust parent pointers for block nbq
    nbq.parent = nbq.parent - 1 + nbq.parity;
    nbq.parity = 1 - nbq.parity;
    blocks[nbq_index] = nbq;
    nbq_index = nbq.nextBlock;
    nbq = blocks[nbq_index];
  }
  else slide = false;

  if ((((*q <= ALPHA_SIZE) && (nbq.first <= ALPHA_SIZE)) ||
        ((*q > ALPHA_SIZE) && (nbq.first > ALPHA_SIZE))) &&
        (nbq.weight = bq.weight + 1)) {
    // merge q into the block of weight one higher
    nodes[*q].block = nbq_index;
    nbq.last = *q;
    if (bq.last == *q) {
      // q's old block disappears
      blocks[bq.prevBlock].nextBlock = bq.nextBlock;
      blocks[bq.nextBlock].prevBlock = bq.prevBlock;
      bq.nextBlock = *availBlock;
      *availBlock = bq_index;
    }
    else {
      if (*q > ALPHA_SIZE) bq.rtChild = findChild(*q - 1, 1, nodes, blocks);
      if (bq.parity == 0) bq.parent = bq.parent - 1;
      bq.parity = 1 - bq.parity;
      bq.first = *q - 1;
    }
  }
  else if (bq.last == *q) {
    if (slide) {
      // q's block is slid forward in the block list
      blocks[bq.nextBlock].prevBlock = bq.prevBlock;
      blocks[bq.prevBlock].nextBlock = bq.nextBlock;
      bq.prevBlock = nbq.prevBlock;
      bq.nextBlock = nbq_index;
      nbq.prevBlock = bq_index;
      blocks[bq.prevBlock].nextBlock = bq_index;
      bq.parent = oldParent;
      bq.parity = oldParity;
    }
    bq.weight = bq.weight + 1;
  }
  else {
    // a new block is created for q
    int b_index = *availBlock;
    block_t b = blocks[b_index];
    *availBlock = blocks[*availBlock].nextBlock;
    nodes[*q].block = b_index;
    b.first = *q;
    b.last = *q;
    if (*q > ALPHA_SIZE) {
      b.rtChild = bq.rtChild;
      bq.rtChild = findChild(*q - 1, 1, nodes, blocks);
      if (b.rtChild == *q - 1) bq.parent = *q;
      else if (bq.parity == 0) bq.parent = bq.parent - 1;
    }
    else if (bq.parity == 0) bq.parent = bq.parent - 1;
    bq.first = *q - 1;
    bq.parity = 1 - bq.parity;
    // insert q's block in its proper place in the block list
    b.prevBlock = nbq.prevBlock;
    b.nextBlock = nbq_index;
    nbq.prevBlock = b_index;
    blocks[b.prevBlock].nextBlock = b_index;
    b.weight = bq.weight + 1;
    b.parent = oldParent;
    b.parity = oldParity;
    blocks[b_index] = b;
  }
  // move q one level higher in tree
  if (*q <= ALPHA_SIZE) *q = oldParent;
  else *q = par;

  blocks[bq_index] = bq;
  blocks[nbq_index] = nbq;
}

int findChild(int j, int parity, node_t *nodes, block_t *blocks) {
  node_t jNode = nodes[j];
  int delta = 2 * (blocks[jNode.block].first - j) + 1 - parity;
  int right = blocks[jNode.block].rtChild;
  int gap = right - blocks[nodes[right].block].last;
  if (delta <= gap) return right - delta;
  else {
    delta = delta - gap - 1;
    right = blocks[blocks[nodes[right].block].prevBlock].first;
    gap = right - blocks[nodes[right].block].last;
    if (delta <= gap) return right - delta;
    else return blocks[blocks[nodes[right].block].prevBlock].first 
      - delta + gap + 1;
  }
}

