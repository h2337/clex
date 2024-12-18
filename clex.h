#ifndef CLEX_H
#define CLEX_H

#include "fa.h"
#include <stdbool.h>

#define CLEX_MAX_RULES 1024

typedef struct clexRule {
  const char *re;
  clexNode *nfa;
  int kind;
} clexRule;

typedef struct clexToken {
  int kind;
  char *lexeme;
} clexToken;

typedef struct clexLexer {
  clexRule **rules;
  const char *content;
  size_t position;
} clexLexer;

clexLexer *clexInit(void);
void clexReset(clexLexer *lexer, const char *content);
bool clexRegisterKind(clexLexer *lexer, const char *re, int kind);
void clexDeleteKinds(clexLexer *lexer);
clexToken clex(clexLexer *lexer);

#endif
