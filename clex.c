#define _CRT_SECURE_NO_WARNINGS
#include "clex.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "fa.h"

clexLexer *clexInit(void) {
  clexLexer *lexer = malloc(sizeof(clexLexer));
  lexer->rules = NULL;
  lexer->content = NULL;
  lexer->position = 0;
  return lexer;
}

void clexLexerDestroy(clexLexer *lexer) {
  if (!lexer) return;
  if (lexer->rules) {
    char **seen = calloc(1024, sizeof(char *));
    for (int i = 0; i < CLEX_MAX_RULES; i++) {
      if (lexer->rules[i]) {
        clexNfaDestroy(lexer->rules[i]->nfa, seen);
        free(lexer->rules[i]);
      }
    }
    free(seen);
    free(lexer->rules);
  }
  free(lexer);
}

void clexReset(clexLexer *lexer, const char *content) {
  lexer->content = content;
  lexer->position = 0;
}

bool clexRegisterKind(clexLexer *lexer, const char *re, int kind) {
  if (!lexer->rules) lexer->rules = calloc(CLEX_MAX_RULES, sizeof(clexRule *));
  for (int i = 0; i < CLEX_MAX_RULES; i++) {
    if (!lexer->rules[i]) {
      clexRule *rule = malloc(sizeof(clexRule));
      rule->re = re;
      rule->nfa = clexNfaFromRe(re, NULL);
      if (!rule->nfa) return false;
      rule->kind = kind;
      lexer->rules[i] = rule;
      break;
    }
  }
  return true;
}

void clexDeleteKinds(clexLexer *lexer) {
  if (lexer->rules) {
    char **seen = calloc(1024, sizeof(char *));
    for (int i = 0; i < CLEX_MAX_RULES; i++) {
      if (lexer->rules[i]) {
        clexNfaDestroy(lexer->rules[i]->nfa, seen);
        free(lexer->rules[i]);
        lexer->rules[i] = NULL;
      }
    }
    free(seen);
  }
}

clexToken clex(clexLexer *lexer) {
  while (isspace(lexer->content[lexer->position])) lexer->position++;
  size_t start = lexer->position;
  while (lexer->content[lexer->position] != '\0' &&
         !isspace(lexer->content[++lexer->position]));
  char *part = calloc(lexer->position - start + 1, sizeof(char));
  strncpy(part, lexer->content + start, lexer->position - start);

  while (strlen(part)) {
    for (int i = 0; i < CLEX_MAX_RULES; i++)
      if (lexer->rules[i])
        if (clexNfaTest(lexer->rules[i]->nfa, part))
          return (clexToken){.lexeme = part, .kind = lexer->rules[i]->kind};
    part[strlen(part) - 1] = '\0';
    lexer->position--;
    if (isspace(lexer->content[lexer->position])) lexer->position--;
  }
  free(part);
  // EOF token is expected to have a kind -1 and a null string for lexeme
  return (clexToken){.lexeme = NULL, .kind = -1};
}
