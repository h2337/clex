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
    for (int i = 0; i < CLEX_MAX_RULES; i++) {
      if (lexer->rules[i]) {
        clexNfaDestroy(lexer->rules[i]->nfa, NULL);
        free(lexer->rules[i]);
      }
    }
    free(lexer->rules);
  }
  free(lexer);
}

void clexReset(clexLexer *lexer, const char *content) {
  lexer->content = content;
  lexer->position = 0;
}

bool clexRegisterKind(clexLexer *lexer, const char *re, int kind) {
  if (!lexer || !re) return false;

  if (!lexer->rules) {
    lexer->rules = calloc(CLEX_MAX_RULES, sizeof(clexRule *));
    if (!lexer->rules) return false;
  }

  for (int i = 0; i < CLEX_MAX_RULES; i++) {
    if (!lexer->rules[i]) {
      clexRule *rule = malloc(sizeof(clexRule));
      if (!rule) return false;
      rule->re = re;
      rule->nfa = clexNfaFromRe(re, NULL);
      if (!rule->nfa) {
        free(rule);
        return false;
      }
      rule->kind = kind;
      lexer->rules[i] = rule;
      return true;
    }
  }

  return false;
}

void clexDeleteKinds(clexLexer *lexer) {
  if (lexer->rules) {
    for (int i = 0; i < CLEX_MAX_RULES; i++) {
      if (lexer->rules[i]) {
        clexNfaDestroy(lexer->rules[i]->nfa, NULL);
        free(lexer->rules[i]);
        lexer->rules[i] = NULL;
      }
    }
  }
}

clexToken clex(clexLexer *lexer) {
  const clexToken eof = {.lexeme = NULL, .kind = -1};

  if (!lexer || !lexer->content || !lexer->rules) return eof;

  const char *content = lexer->content;
  size_t length = strlen(content);

  while (lexer->position < length &&
         isspace((unsigned char)content[lexer->position]))
    lexer->position++;

  if (lexer->position >= length) return eof;

  size_t start = lexer->position;
  size_t end = start;
  while (content[end] != '\0' && !isspace((unsigned char)content[end])) end++;

  size_t partLength = end - start;
  if (partLength == 0) {
    lexer->position = end;
    return eof;
  }

  char *part = calloc(partLength + 1, sizeof(char));
  if (!part) return eof;
  memcpy(part, content + start, partLength);

  size_t activeLength = partLength;
  while (activeLength > 0) {
    for (int i = 0; i < CLEX_MAX_RULES; i++) {
      clexRule *rule = lexer->rules[i];
      if (rule && clexNfaTest(rule->nfa, part)) {
        lexer->position = start + activeLength;
        return (clexToken){.lexeme = part, .kind = rule->kind};
      }
    }
    activeLength--;
    part[activeLength] = '\0';
  }

  free(part);
  lexer->position = end;
  return eof;
}
