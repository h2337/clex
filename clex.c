#include "clex.h"
#include "fa.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
  char *re;
  Node *nfa;
  int kind;
} Rule;

Rule **rules;
char *clexContent;
size_t clexPosition;

void initClex(char *content) {
  clexContent = content;
  clexPosition = 0;
}

void registerKind(char *re, int kind) {
  if (!rules)
    rules = calloc(1024, sizeof(Rule *));
  for (int i = 0; i < 1024; i++) {
    if (!rules[i]) {
      Rule *rule = malloc(sizeof(Rule));
      rule->re = re;
      rule->nfa = reToNFA(re);
      rule->kind = kind;
      rules[i] = rule;
      break;
    }
  }
}

void deleteKinds() {
  rules = calloc(1024, sizeof(Rule *));
}

Token *clex() {
  size_t start = clexPosition;
  while (!isspace(clexContent[clexPosition++]) && clexContent[clexPosition - 1] != '\0');
  char *part = calloc(clexPosition - start, sizeof(char));
  strncpy(part, clexContent + start, clexPosition - start - 1);

  while (strlen(part)) {
    for (int i = 0; i < 1024; i++) {
      if (rules[i]) {
        bool matches = test(rules[i]->nfa, part);
        if (matches) {
          Token *token = malloc(sizeof(Token));
          token->lexeme = part;
          token->kind = rules[i]->kind;
          return token;
        }
      }
    }
    part[strlen(part) - 1] = '\0';
    while (!isspace(clexContent[clexPosition]))
      clexPosition--;
    clexPosition--;
  }
}

