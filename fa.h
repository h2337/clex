#ifndef CLEX_FA_H
#define CLEX_FA_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct clexNode clexNode;

typedef struct clexTransition {
  char fromValue;
  char toValue;
  clexNode* to;
} clexTransition;

typedef struct clexNode {
  bool isStart;
  bool isFinish;
  clexTransition** transitions;
} clexNode;

typedef struct clexReLexerState {
  const char* lexerContent;
  size_t lexerPosition;
  clexNode* lastBeforeParanEntry;
  clexNode* beforeParanEntry;
  clexNode* paranEntry;
  bool inPipe;
  bool pipeSeen;
  bool inBackslash;
} clexReLexerState;

clexNode* clexNfaFromRe(const char* re, clexReLexerState* state);
bool clexNfaTest(clexNode* nfa, const char* target);
void clexNfaDraw(clexNode* nfa);
void clexNfaDestroy(clexNode* nfa, clexNode** seen);

#endif
