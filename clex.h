#ifndef CLEX_H
#define CLEX_H

typedef struct Token {
  int kind;
  char *lexeme;
} Token;

void initClex(const char *content);
void registerKind(const char *re, int kind);
void deleteKinds(void);
Token clex(void);

#endif
