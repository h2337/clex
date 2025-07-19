#ifndef FA_H
#define FA_H

#include <stdbool.h>

typedef struct Node Node;

typedef struct Transition {
  char fromValue;
  char toValue;
  Node *to;
} Transition;

typedef struct Node {
  bool isStart;
  bool isFinish;
  Transition **transitions;
} Node;

Node *reToNFA(const char *re);
bool test(Node *nfa, const char *target);
void drawNFA(Node *nfa);

#endif
