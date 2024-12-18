#ifndef CLEX_H
#define CLEX_H

typedef struct Token {
  int kind;
  char *lexeme;
} Token;

void initClex(char *content);
void registerKind(char *re, int kind);
void deleteKinds(void);
Token clex(void);

#endif
