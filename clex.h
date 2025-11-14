#ifndef CLEX_H
#define CLEX_H

#include <stdbool.h>
#include <stddef.h>

#include "fa.h"

#define CLEX_MAX_RULES 1024

typedef struct clexRule {
  const char *re;
  clexNode *nfa;
  int kind;
} clexRule;

typedef struct clexToken {
  int kind;
  char *lexeme;
  size_t start;
  size_t linen;
  size_t linepos;
} clexToken;

typedef struct clexLexer {
  clexRule **rules;
  const char *content;
  size_t position;
  size_t linen;
  size_t linepos;
} clexLexer;

clexLexer *clexInit(void);
void clexLexerDestroy(clexLexer *lexer);
void clexReset(clexLexer *lexer, const char *content);
bool clexRegisterKind(clexLexer *lexer, const char *re, int kind);
void clexDeleteKinds(clexLexer *lexer);
clexToken clex(clexLexer *lexer);

#endif
