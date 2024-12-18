#define _CRT_SECURE_NO_WARNINGS
#include "clex.h"
#include "fa.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
  const char *re;
  Node *nfa;
  int kind;
} Rule;

static Rule **rules;
static const char *clexContent;
static size_t clexPosition;

void clexInit(const char *content) {
  clexContent = content;
  clexPosition = 0;
}

bool clexRegisterKind(const char *re, int kind) {
  if (!rules) rules = calloc(CLEX_MAX_RULES, sizeof(Rule *));
  for (int i = 0; i < CLEX_MAX_RULES; i++) {
    if (!rules[i]) {
      Rule *rule = malloc(sizeof(Rule));
      rule->re = re;
      rule->nfa = NFAFromRe(re);
      if (!rule->nfa) return false;
      rule->kind = kind;
      rules[i] = rule;
      break;
    }
  }
  return true;
}

void clexDeleteKinds() {
  if (rules) memset(rules, 0, CLEX_MAX_RULES);
}

Token clex() {
  while (isspace(clexContent[clexPosition])) clexPosition++;
  size_t start = clexPosition;
  while (!isspace(clexContent[++clexPosition]) && clexContent[clexPosition] != '\0');
  char *part = calloc(clexPosition - start + 1, sizeof(char));
  strncpy(part, clexContent + start, clexPosition - start);

  while (strlen(part)) {
    for (int i = 0; i < 1024; i++) {
      if (rules[i]) {
        bool matches = NFATest(rules[i]->nfa, part);
        if (matches) {
          return (Token){.lexeme = part, .kind = rules[i]->kind};
        }
      }
    }
    part[strlen(part) - 1] = '\0';
    clexPosition--;
    if (isspace(clexContent[clexPosition]))
      clexPosition--;
  }
  // EOF token is expected to have a kind=0 and a null string.
  return (Token){.lexeme = NULL, .kind = 0};
}
