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

Transition *makeTransition(char fromValue, char toValue, Node *to) {
  Transition *result = malloc(sizeof(Transition));
  result->fromValue = fromValue;
  result->toValue = toValue;
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
  BSLASH,
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
bool inBackslash = false;
char **drawSeen = NULL;
char **getFinishNodeSeen = NULL;

void initLexer(char *content) {
  lexerContent = content;
  lexerPosition = 0;
  beforeParanEntry = NULL;
  paranEntry = NULL;
  inPipe = false;
  pipeSeen = false;
  drawSeen = NULL;
  getFinishNodeSeen = NULL;
}

Token *makeToken(TokenKind kind) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  return result;
}

Token *makeLexemeToken(TokenKind kind, char lexeme) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  result->lexeme = lexeme;
  return result;
}

Token *lex() {
  if (lexerContent[lexerPosition] == '\0')
    return makeToken(EOF);
  if (lexerContent[lexerPosition] == '(') {
    lexerPosition++;
    return makeLexemeToken(OPARAN, '(');
  }
  if (lexerContent[lexerPosition] == ')') {
    lexerPosition++;
    return makeLexemeToken(CPARAN, ')');
  }
  if (lexerContent[lexerPosition] == '[') {
    lexerPosition++;
    return makeLexemeToken(OSBRACKET, '[');
  }
  if (lexerContent[lexerPosition] == ']') {
    lexerPosition++;
    return makeLexemeToken(CSBRACKET, ']');
  }
  if (lexerContent[lexerPosition] == '-') {
    lexerPosition++;
    return makeLexemeToken(DASH, '-');
  }
  if (lexerContent[lexerPosition] == '|') {
    lexerPosition++;
    return makeLexemeToken(PIPE, '|');
  }
  if (lexerContent[lexerPosition] == '*') {
    lexerPosition++;
    return makeLexemeToken(STAR, '*');
  }
  if (lexerContent[lexerPosition] == '+') {
    lexerPosition++;
    return makeLexemeToken(PLUS, '+');
  }
  if (lexerContent[lexerPosition] == '?') {
    lexerPosition++;
    return makeLexemeToken(QUESTION, '?');
  }
  if (lexerContent[lexerPosition] == '\\') {
    lexerPosition++;
    return makeLexemeToken(BSLASH, '\\');
  }
  Token *result = makeLexemeToken(LITERAL, lexerContent[lexerPosition]);
  lexerPosition++;
  return result;
}

Token *peek() {
  Token *lexed = lex();
  if (lexed->kind != EOF)
    lexerPosition--;
  return lexed;
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

char *drawKey(Node *node1, Node *node2, char fromValue, char toValue) {
  char *result = malloc(1024);
  sprintf(result, "%p%p%c%c", node1, node2, fromValue, toValue);
  return result;
}

char *getFinishNodeKey(Node *node) {
  char *result = malloc(1024);
  sprintf(result, "%p", node);
  return result;
}

void drawNFA(Node *nfa) {
  if (!drawSeen)
    drawSeen = calloc(1024, sizeof(char *));
  for (int i = 0; i < 100; i++)
    if (nfa->transitions[i]) {
      printf("%d -> %d [label=\"%c-%c\"];\n", nfa, nfa->transitions[i]->to, nfa->transitions[i]->fromValue, nfa->transitions[i]->toValue);
      if (!inArray(drawSeen, drawKey(nfa, nfa->transitions[i]->to, nfa->transitions[i]->fromValue, nfa->transitions[i]->toValue))) {
        insertArray(drawSeen, drawKey(nfa, nfa->transitions[i]->to, nfa->transitions[i]->fromValue, nfa->transitions[i]->toValue));
        drawNFA(nfa->transitions[i]->to);
      }
    }
}

Node *getFinishNode(Node *node) {
  if (!getFinishNodeSeen)
    getFinishNodeSeen = calloc(1024, sizeof(char *));
  if (node->isFinish)
    return node;
  for (int i = 0; i < 100; i++)
    if (node->transitions[i] != NULL)
      if (!inArray(getFinishNodeSeen, getFinishNodeKey(node->transitions[i]->to))) {
        insertArray(getFinishNodeSeen, getFinishNodeKey(node->transitions[i]->to));
        return getFinishNode(node->transitions[i]->to);
      }
  return NULL;
}

Node *reToNFA(char *re) {
  if (re) initLexer(re);
  Token *token;
  Node *entry = makeNode(true, true);
  Node *last = entry;
  while ((token = lex())->kind != EOF) {
    getFinishNodeSeen = NULL;
    if (inBackslash) {
      inBackslash = false;
      Node *node = makeNode(false, true);
      last->transitions[0] = makeTransition(token->lexeme, token->lexeme, node);
      last->isFinish = false;
      last = node;
      continue;
    }
    if (peek()->kind == OPARAN) {
      beforeParanEntry = last;
    }
    if (token->kind == BSLASH) {
      inBackslash = true;
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
      last->transitions[0] = makeTransition(token->lexeme, token->lexeme, node);
      last->isFinish = false;
      last = node;
    }
    if (token->kind == PIPE) {
      inPipe = true;
      if (!paranEntry) {
        Node *pastEntry = entry;
        pastEntry->isStart = false;

        entry = makeNode(true, false);

        entry->transitions[0] = makeTransition('\0', '\0', pastEntry);
        Node *firstFinish = getFinishNode(pastEntry);
        firstFinish->isFinish = false;

        Node *second = reToNFA(NULL);
        Node *secondFinish = getFinishNode(second);
        secondFinish->isFinish = false;
        entry->transitions[1] = makeTransition('\0', '\0', second);

        Node *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        secondFinish->transitions[0] = makeTransition('\0', '\0', finish);

        last = finish;
      } else {
        Node *pipeEntry = makeNode(beforeParanEntry ? false : true, false);
        if (beforeParanEntry) {
          beforeParanEntry->transitions[0] = makeTransition(beforeParanEntry->transitions[0]->fromValue, beforeParanEntry->transitions[0]->toValue, pipeEntry);
          beforeParanEntry = pipeEntry;
        } else {
          entry = pipeEntry;
          beforeParanEntry = entry;
        }
        pipeEntry->transitions[0] = makeTransition('\0', '\0', paranEntry);

        Node *firstFinish = getFinishNode(paranEntry);
        firstFinish->isFinish = false;

        Node *second = reToNFA(NULL);
        Node *secondFinish = getFinishNode(second);
        secondFinish->isFinish = false;
        pipeEntry->transitions[1] = makeTransition('\0', '\0', second);

        Node *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        secondFinish->transitions[0] = makeTransition('\0', '\0', finish);

        last = finish;
      }
    }
    if (token->kind == STAR) {
      if (!paranEntry) {
        Node *pastEntry = entry;
        pastEntry->isStart = false;

        Node *finish = makeNode(false, true);
        entry = makeNode(true, false);

        entry->transitions[0] = makeTransition('\0', '\0', pastEntry);
        entry->transitions[1] = makeTransition('\0', '\0', finish);
        Node *firstFinish = getFinishNode(pastEntry);
        firstFinish->isFinish = false;
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        firstFinish->transitions[1] = makeTransition('\0', '\0', pastEntry);

        last = finish;
      } else {
        Node *starEntry = makeNode(beforeParanEntry ? false : true, false);
        if (beforeParanEntry)
          beforeParanEntry->transitions[0] = makeTransition(beforeParanEntry->transitions[0]->fromValue, beforeParanEntry->transitions[0]->toValue, starEntry);
        else
          entry = starEntry;

        Node *finish = makeNode(false, true);

        starEntry->transitions[0] = makeTransition('\0', '\0', paranEntry);
        starEntry->transitions[1] = makeTransition('\0', '\0', finish);
        Node *firstFinish = getFinishNode(paranEntry);
        firstFinish->isFinish = false;
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        firstFinish->transitions[1] = makeTransition('\0', '\0', beforeParanEntry && pipeSeen ? beforeParanEntry : starEntry);

        last = finish;
      }
    }
    if (token->kind == PLUS) {
      Node *finish = getFinishNode(entry);
      finish->transitions[1] = makeTransition('\0', '\0', beforeParanEntry ? beforeParanEntry : entry);
    }
    if (token->kind == QUESTION) {
      if (!paranEntry) {
        Node *pastEntry = entry;
        pastEntry->isStart = false;

        entry = makeNode(true, false);

        entry->transitions[0] = makeTransition('\0', '\0', pastEntry);
        Node *firstFinish = getFinishNode(pastEntry);
        firstFinish->isFinish = false;

        Node *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        entry->transitions[1] = makeTransition('\0', '\0', finish);

        last = finish;
      } else {
        Node *questionEntry = makeNode(beforeParanEntry ? false : true, false);
        if (beforeParanEntry)
          beforeParanEntry->transitions[0] = makeTransition(beforeParanEntry->transitions[0]->fromValue, beforeParanEntry->transitions[0]->toValue, questionEntry);
        else
          entry = questionEntry;

        questionEntry->transitions[0] = makeTransition('\0', '\0', paranEntry);
        Node *firstFinish = getFinishNode(paranEntry);
        firstFinish->isFinish = false;

        Node *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        questionEntry->transitions[1] = makeTransition('\0', '\0', finish);

        last = finish;
      }
    }
    if (token->kind == OSBRACKET) {
      int index = 0;
      Node *node = makeNode(false, true);
      while (peek()->kind != CSBRACKET) {
        char fromValue = lex()->lexeme;
        if (peek()->kind == DASH) {
          lex();
          char toValue = lex()->lexeme;
          last->transitions[index++] = makeTransition(fromValue, toValue, node);
        } else {
          last->transitions[index++] = makeTransition(fromValue, fromValue, node);
        }
      }
      lex();
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
        if (nfa->transitions[j]->fromValue <= target[i] && nfa->transitions[j]->toValue >= target[i]) {
          if (test(nfa->transitions[j]->to, target + i + 1))
            return true;
        } else if (nfa->transitions[j]->fromValue == '\0') {
          if (test(nfa->transitions[j]->to, target + i))
            return true;
        }
      }
    return false;
  }
  for (int j = 0; j < 100; j++) {
    if (nfa->transitions[j] && nfa->transitions[j]->fromValue == '\0')
      if (test(nfa->transitions[j]->to, ""))
        return true;
  }
  if (nfa->isFinish)
    return true;
  return false;
}

