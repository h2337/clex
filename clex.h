#ifndef CLEX_H
#define CLEX_H

typedef struct Token {
  int kind;
  char *lexeme;
} Token;

void clexInit(const char *content);
void clexRegisterKind(const char *re, int kind);
void clexDeleteKinds(void);
Token clex(void);

#endif
