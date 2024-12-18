#ifndef FA_H
#define FA_H

#include <stdbool.h>

typedef struct Node Node;

typedef struct Transition {
  char value;
  Node *to;
} Transition;

typedef struct Node {
  bool isStart;
  bool isFinish;
  Transition **transitions;
} Node;

void initLexer(char *content);
Node *reToNFA();
bool test(Node *nfa, char *target);

#endif
