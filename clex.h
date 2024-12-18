#ifndef CLEX_H
#define CLEX_H

#include <stdbool.h>

#define CLEX_MAX_RULES 1024

typedef struct Token {
  int kind;
  char *lexeme;
} Token;

void clexInit(const char *content);
bool clexRegisterKind(const char *re, int kind);
void clexDeleteKinds(void);
Token clex(void);

#endif
