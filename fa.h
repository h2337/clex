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

Node *NFAFromRe(const char *re);
bool NFATest(Node *nfa, const char *target);
void NFADraw(Node *nfa);

#endif
