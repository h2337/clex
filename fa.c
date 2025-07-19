#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#undef EOF

typedef struct Node Node;

typedef struct Transition {
  char value;
  Node *to;
} Transition;

typedef struct Node {
  bool isStart;
  bool isFinish;
  Transition **transitions;
} Node;

Node *makeNode(bool isStart, bool isFinish) {
  Node *result = malloc(sizeof(Node));
  result->isStart = isStart;
  result->isFinish = isFinish;
  result->transitions = calloc(100, sizeof(Transition *));
  return result;
}

Transition *makeTransition(char value, Node *to) {
  Transition *result = malloc(sizeof(Transition));
  result->value = value;
  result->to = to;
  return result;
}

typedef enum TokenKind {
  OPARAN,
  CPARAN,
  PIPE,
  STAR,
  LITERAL,
  EOF,
} TokenKind;

typedef struct Token {
  TokenKind kind;
  char lexeme;
} Token;

char *lexerContent;
size_t lexerPosition;

void initLexer(char *content) {
  lexerContent = content;
  lexerPosition = 0;
}

Token *makeToken(TokenKind kind) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  return result;
}

Token *makeLiteralToken(TokenKind kind, char literal) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  result->lexeme = literal;
  return result;
}

Token *lex() {
  if (lexerContent[lexerPosition] == '\0')
    return makeToken(EOF);
  if (lexerContent[lexerPosition] == '(') {
    lexerPosition++;
    return makeToken(OPARAN);
  }
  if (lexerContent[lexerPosition] == ')') {
    lexerPosition++;
    return makeToken(CPARAN);
  }
  if (lexerContent[lexerPosition] == '|') {
    lexerPosition++;
    return makeToken(PIPE);
  }
  if (lexerContent[lexerPosition] == '*') {
    lexerPosition++;
    return makeToken(STAR);
  }
  Token *result = makeLiteralToken(LITERAL, lexerContent[lexerPosition]);
  lexerPosition++;
  return result;
}

Node *getFinishNode(Node *node) {
  if (node->isFinish)
    return node;
  for (int i = 0; i < 100; i++)
    if (node->transitions[i] != NULL)
      return getFinishNode(node->transitions[i]->to);
  return NULL;
}

Node *reToNFA() {
  Token *token;
  Node *entry = makeNode(true, true);
  Node *last = entry;
  while ((token = lex())->kind != EOF) {
    if (token->kind == LITERAL) {
      Node *node = makeNode(false, true);
      last->transitions[0] = makeTransition(token->lexeme, node);
      last->isFinish = false;
      last = node;
    }
    if (token->kind == PIPE) {
      Node *pastEntry = entry;

      entry = makeNode(true, false);

      entry->transitions[0] = makeTransition('\0', pastEntry);
      Node *firstFinish = getFinishNode(pastEntry);
      firstFinish->isFinish = false;

      Node *second = reToNFA();
      Node *secondFinish = getFinishNode(second);
      secondFinish->isFinish = false;
      entry->transitions[1] = makeTransition('\0', second);

      Node *finish = makeNode(false, true);
      firstFinish->transitions[0] = makeTransition('\0', finish);
      secondFinish->transitions[0] = makeTransition('\0', finish);
    }
  }
  return entry;
}

bool test(Node *nfa, char *target) {
  for (int i = 0; i < strlen(target); i++) {
    for (int j = 0; j < 100; j++)
      if (nfa->transitions[j]) {
        if (nfa->transitions[j]->value == target[i]) {
          if (test(nfa->transitions[j]->to, target + i + 1))
            return true;
        } else if (nfa->transitions[j]->value == '\0') {
          if (test(nfa->transitions[j]->to, target + i))
            return true;
        }
      }
    return false;
  }
  if (nfa->transitions[0] && nfa->transitions[0]->value == '\0' && nfa->transitions[0]->to->isFinish)
    return true;
  if (nfa->isFinish)
    return true;
  return false;
}

void drawNFA(Node *nfa) {
  for (int i = 0; i < 100; i++)
    if (nfa->transitions[i]) {
      printf("%d -> %d [label=\"%c\"];\n", nfa, nfa->transitions[i]->to, nfa->transitions[i]->value);
      drawNFA(nfa->transitions[i]->to);
    }
}

int main(int argc, char *argv[]) {
  char *re = argv[1];
  initLexer(re);
  Node *nfa = reToNFA();
  char *target = argv[2];
  printf("%d\n", test(nfa, target));
  drawNFA(nfa);
}
