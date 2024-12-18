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

void initLexer(char *content);
Node *reToNFA();
bool test(Node *nfa, char *target);
void drawNFA(Node *nfa);

#endif
