#include "fa.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#undef EOF

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
Node *beforeParanEntry = NULL;
Node *paranEntry = NULL;
bool inPipe = false;
bool pipeSeen = false;

void initLexer(char *content) {
  lexerContent = content;
  lexerPosition = 0;
  beforeParanEntry = NULL;
  paranEntry = NULL;
  inPipe = false;
  pipeSeen = false;
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
  if (lexed->kind != EOF)
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
    if (peek()->kind == OPARAN) {
      beforeParanEntry = last;
    }
    if (token->kind == OPARAN) {
      paranEntry = last;
    }
    if (token->kind == CPARAN) {
      if (inPipe) {
        inPipe = false;
        pipeSeen = true;
        return entry;
      }
    }
    if (token->kind == LITERAL) {
      Node *node = makeNode(false, true);
      last->transitions[0] = makeTransition(token->lexeme, node);
      last->isFinish = false;
      last = node;
    }
    if (token->kind == PIPE) {
      inPipe = true;
      if (!paranEntry) {
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
      } else {
        Node *pipeEntry = makeNode(beforeParanEntry ? false : true, false);
        if (beforeParanEntry) {
          beforeParanEntry->transitions[0] = makeTransition(beforeParanEntry->transitions[0]->value, pipeEntry);
          beforeParanEntry = pipeEntry;
        } else
          entry = pipeEntry;
        pipeEntry->transitions[0] = makeTransition('\0', paranEntry);

        Node *firstFinish = getFinishNode(paranEntry);
        firstFinish->isFinish = false;

        Node *second = reToNFA();
        Node *secondFinish = getFinishNode(second);
        secondFinish->isFinish = false;
        pipeEntry->transitions[1] = makeTransition('\0', second);

        Node *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', finish);
        secondFinish->transitions[0] = makeTransition('\0', finish);

        last = finish;
      }
    }
    if (token->kind == STAR) {
      if (!paranEntry) {
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
      } else {
        Node *starEntry = makeNode(beforeParanEntry ? false : true, false);
        if (beforeParanEntry)
          beforeParanEntry->transitions[0] = makeTransition(beforeParanEntry->transitions[0]->value, starEntry);
        else
          entry = starEntry;

        Node *finish = makeNode(false, true);

        starEntry->transitions[0] = makeTransition('\0', paranEntry);
        starEntry->transitions[1] = makeTransition('\0', finish);
        Node *firstFinish = getFinishNode(paranEntry);
        firstFinish->isFinish = false;
        firstFinish->transitions[0] = makeTransition('\0', finish);
        firstFinish->transitions[1] = makeTransition('\0', beforeParanEntry && pipeSeen ? beforeParanEntry : starEntry);

        last = finish;
      }
    }
    if (token->kind == PLUS) {
      Node *finish = getFinishNode(entry);
      finish->transitions[1] = makeTransition('\0', beforeParanEntry ? beforeParanEntry : entry);
    }
    if (token->kind == QUESTION) {
      if (!paranEntry) {
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
      } else {
        Node *questionEntry = makeNode(beforeParanEntry ? false : true, false);
        if (beforeParanEntry)
          beforeParanEntry->transitions[0] = makeTransition(beforeParanEntry->transitions[0]->value, questionEntry);
        else
          entry = questionEntry;

        questionEntry->transitions[0] = makeTransition('\0', paranEntry);
        Node *firstFinish = getFinishNode(paranEntry);
        firstFinish->isFinish = false;

        Node *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', finish);
        questionEntry->transitions[1] = makeTransition('\0', finish);

        last = finish;
      }
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
  for (int j = 0; j < 100; j++) {
    if (nfa->transitions[j] && nfa->transitions[j]->value == '\0' && nfa->transitions[j]->to->isFinish)
      return true;
  }
  if (nfa->isFinish)
    return true;
  return false;
}

bool inArray(char **array, char *key) {
  for (int i = 0; i < 1024; i++)
    if (array[i] && strcmp(array[i], key) == 0)
      return true;
  return false;
}

void insertArray(char **array, char *key) {
  for (int i = 0; i < 1024; i++)
    if (!array[i]) {
      array[i] = key;
      return;
    }
}

char *key(Node *node1, Node *node2, char value) {
  char *result = malloc(1024);
  sprintf(result, "%p%p%c", node1, node2, value);
  return result;
}

char **seen;

void drawNFA(Node *nfa) {
  for (int i = 0; i < 100; i++)
    if (nfa->transitions[i]) {
      printf("%d -> %d [label=\"%c\"];\n", nfa, nfa->transitions[i]->to, nfa->transitions[i]->value);
      if (!inArray(seen, key(nfa, nfa->transitions[i]->to, nfa->transitions[i]->value))) {
        insertArray(seen, key(nfa, nfa->transitions[i]->to, nfa->transitions[i]->value));
        drawNFA(nfa->transitions[i]->to);
      }
    }
}

