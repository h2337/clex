#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

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
  OSBRACKET,
  CSBRACKET,
  DASH,
  PIPE,
  STAR,
  PLUS,
  QUESTION,
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
  if (lexerContent[lexerPosition] == '[') {
    lexerPosition++;
    return makeToken(OSBRACKET);
  }
  if (lexerContent[lexerPosition] == ']') {
    lexerPosition++;
    return makeToken(CSBRACKET);
  }
  if (lexerContent[lexerPosition] == '-') {
    lexerPosition++;
    return makeToken(DASH);
  }
  if (lexerContent[lexerPosition] == '|') {
    lexerPosition++;
    return makeToken(PIPE);
  }
  if (lexerContent[lexerPosition] == '*') {
    lexerPosition++;
    return makeToken(STAR);
  }
  if (lexerContent[lexerPosition] == '+') {
    lexerPosition++;
    return makeToken(PLUS);
  }
  if (lexerContent[lexerPosition] == '?') {
    lexerPosition++;
    return makeToken(QUESTION);
  }
  Token *result = makeLiteralToken(LITERAL, lexerContent[lexerPosition]);
  lexerPosition++;
  return result;
}

Token *peek() {
  Token *lexed = lex();
  lexerPosition--;
  return lexed;
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
      pastEntry->isStart = false;

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

      last = finish;
    }
    if (token->kind == STAR) {
      Node *pastEntry = entry;
      pastEntry->isStart = false;

      Node *finish = makeNode(false, true);
      entry = makeNode(true, false);

      entry->transitions[0] = makeTransition('\0', pastEntry);
      entry->transitions[1] = makeTransition('\0', finish);
      Node *firstFinish = getFinishNode(pastEntry);
      firstFinish->isFinish = false;
      firstFinish->transitions[0] = makeTransition('\0', finish);
      firstFinish->transitions[1] = makeTransition('\0', pastEntry);

      last = finish;
    }
    if (token->kind == PLUS) {
      Node *finish = getFinishNode(entry);
      finish->transitions[1] = makeTransition('\0', entry);
    }
    if (token->kind == QUESTION) {
      Node *pastEntry = entry;
      pastEntry->isStart = false;

      entry = makeNode(true, false);

      entry->transitions[0] = makeTransition('\0', pastEntry);
      Node *firstFinish = getFinishNode(pastEntry);
      firstFinish->isFinish = false;

      Node *finish = makeNode(false, true);
      firstFinish->transitions[0] = makeTransition('\0', finish);
      entry->transitions[1] = makeTransition('\0', finish);

      last = finish;
    }
    if (token->kind == OSBRACKET) {
      char *values = calloc(1000, sizeof(char));
      int valuesIndex = -1;
      while (peek()->kind != CSBRACKET) {
        char value = lex()->lexeme;
        values[++valuesIndex] = value;
        if (peek()->kind == DASH) {
          lex();
          char endValue = lex()->lexeme;
          for (int i = values[valuesIndex]; i <= endValue; i++)
            values[++valuesIndex] = i;
        }
      }
      Node *node = makeNode(false, true);
      int index = 0;
      while (*values != '\0') {
        last->transitions[index++] = makeTransition(values[0], node);
        values++;
      }
      last->isFinish = false;
      last = node;
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
#ifdef TEST
  initLexer("a");
  Node *nfa = reToNFA();
  assert(test(nfa, "a") == true);
  assert(test(nfa, "b") == false);

  initLexer("ab");
  nfa = reToNFA();
  assert(test(nfa, "ab") == true);
  assert(test(nfa, "a") == false);
  assert(test(nfa, "b") == false);

  initLexer("ab|c");
  nfa = reToNFA();
  assert(test(nfa, "ab") == true);
  assert(test(nfa, "c") == true);
  assert(test(nfa, "abc") == false);

  initLexer("ab*c");
  nfa = reToNFA();
  assert(test(nfa, "c") == true);
  assert(test(nfa, "abc") == true);
  assert(test(nfa, "ababc") == true);
  assert(test(nfa, "ab") == false);
  assert(test(nfa, "abd") == false);
  assert(test(nfa, "acc") == false);

  initLexer("ab+c");
  nfa = reToNFA();
  assert(test(nfa, "c") == false);
  assert(test(nfa, "abc") == true);
  assert(test(nfa, "ababc") == true);
  assert(test(nfa, "ab") == false);
  assert(test(nfa, "abd") == false);
  assert(test(nfa, "acc") == false);

  initLexer("ab?c");
  nfa = reToNFA();
  assert(test(nfa, "c") == true);
  assert(test(nfa, "abc") == true);
  assert(test(nfa, "ababc") == false);
  assert(test(nfa, "ab") == false);
  assert(test(nfa, "abd") == false);
  assert(test(nfa, "acc") == false);

  initLexer("[ab]c");
  nfa = reToNFA();
  assert(test(nfa, "c") == false);
  assert(test(nfa, "ac") == true);
  assert(test(nfa, "bc") == true);
  assert(test(nfa, "abc") == false);
  assert(test(nfa, "bd") == false);
  assert(test(nfa, "acc") == false);

  initLexer("[A-Za-z]c");
  nfa = reToNFA();
  assert(test(nfa, "c") == false);
  assert(test(nfa, "ac") == true);
  assert(test(nfa, "bc") == true);
  assert(test(nfa, "Ac") == true);
  assert(test(nfa, "Zd") == false);
  assert(test(nfa, "Zc") == true);
#endif
#ifndef TEST
  char *re = argv[1];
  initLexer(re);
  Node *nfa = reToNFA();
  char *target = argv[2];
  printf("%d\n", test(nfa, target));
#endif
}
