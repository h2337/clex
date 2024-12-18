#include "fa.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef EOF

static clexNode *makeNode(bool isStart, bool isFinish) {
  clexNode *result = malloc(sizeof(clexNode));
  result->isStart = isStart;
  result->isFinish = isFinish;
  result->transitions = calloc(100, sizeof(clexTransition *));
  return result;
}

static clexTransition *makeTransition(char fromValue, char toValue,
                                      clexNode *to) {
  clexTransition *result = malloc(sizeof(clexTransition));
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

static Token *makeToken(TokenKind kind) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  return result;
}

static Token *makeLexemeToken(TokenKind kind, char lexeme) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  result->lexeme = lexeme;
  return result;
}

static Token *lex(clexReLexerState *state) {
  switch (state->lexerContent[state->lexerPosition]) {
  case '\0':
    return makeToken(EOF);
  case '(':
    state->lexerPosition++;
    return makeLexemeToken(OPARAN, '(');
  case ')':
    state->lexerPosition++;
    return makeLexemeToken(CPARAN, ')');
  case '[':
    state->lexerPosition++;
    return makeLexemeToken(OSBRACKET, '[');
  case ']':
    state->lexerPosition++;
    return makeLexemeToken(CSBRACKET, ']');
  case '-':
    state->lexerPosition++;
    return makeLexemeToken(DASH, '-');
  case '|':
    state->lexerPosition++;
    return makeLexemeToken(PIPE, '|');
  case '*':
    state->lexerPosition++;
    return makeLexemeToken(STAR, '*');
  case '+':
    state->lexerPosition++;
    return makeLexemeToken(PLUS, '+');
  case '?':
    state->lexerPosition++;
    return makeLexemeToken(QUESTION, '?');
  case '\\':
    state->lexerPosition++;
    return makeLexemeToken(BSLASH, '\\');
  }
  Token *result =
      makeLexemeToken(LITERAL, state->lexerContent[state->lexerPosition]);
  state->lexerPosition++;
  return result;
}

static Token *peek(clexReLexerState *state) {
  Token *lexed = lex(state);
  if (lexed->kind != EOF)
    state->lexerPosition--;
  return lexed;
}

static bool inArray(char **array, char *key) {
  for (int i = 0; i < 1024; i++)
    if (array[i] && strcmp(array[i], key) == 0)
      return true;
  return false;
}

static void insertArray(char **array, char *key) {
  for (int i = 0; i < 1024; i++)
    if (!array[i]) {
      array[i] = key;
      return;
    }
}

static char *getFinishNodeKey(clexNode *node) {
  char *result = malloc(1024);
  sprintf(result, "%p", node);
  return result;
}

static clexNode *getFinishNode(clexNode *node, char **getFinishNodeSeen) {
  if (!getFinishNodeSeen)
    getFinishNodeSeen = calloc(1024, sizeof(char *));
  if (node->isFinish)
    return node;
  for (int i = 0; i < 100; i++)
    if (node->transitions[i] != NULL)
      if (!inArray(getFinishNodeSeen,
                   getFinishNodeKey(node->transitions[i]->to))) {
        insertArray(getFinishNodeSeen,
                    getFinishNodeKey(node->transitions[i]->to));
        return getFinishNode(node->transitions[i]->to, getFinishNodeSeen);
      }
  return NULL;
}

clexNode *clexNfaFromRe(const char *re, clexReLexerState *state) {
  if (!state)
    state = calloc(1, sizeof(clexReLexerState));
  if (re) {
    state->lexerContent = re;
    state->lexerPosition = 0;
    state->lastBeforeParanEntry = NULL;
    state->beforeParanEntry = NULL;
    state->paranEntry = NULL;
    state->inPipe = false;
    state->pipeSeen = false;
    state->inBackslash = false;
    state->getFinishNodeSeen = NULL;
  }

  Token *token;
  clexNode *entry = makeNode(true, true);
  clexNode *last = entry;
  while ((token = lex(state))->kind != EOF) {
    state->getFinishNodeSeen = NULL;
    if (state->inBackslash) {
      state->inBackslash = false;
      clexNode *node = makeNode(false, true);
      last->transitions[0] = makeTransition(token->lexeme, token->lexeme, node);
      last->isFinish = false;
      last = node;
      continue;
    }
    if (peek(state)->kind == OPARAN) {
      state->lastBeforeParanEntry = state->beforeParanEntry;
      state->beforeParanEntry = last;
    }
    if (token->kind == BSLASH) {
      state->inBackslash = true;
    }
    if (token->kind == OPARAN) {
      state->paranEntry = last;
    }
    if (token->kind == CPARAN) {
      if (state->inPipe) {
        state->inPipe = false;
        state->pipeSeen = true;
        return entry;
      }
    }
    if (token->kind == LITERAL) {
      clexNode *node = makeNode(false, true);
      last->transitions[0] = makeTransition(token->lexeme, token->lexeme, node);
      last->isFinish = false;
      last = node;
    }
    if (token->kind == PIPE) {
      state->inPipe = true;
      if (!state->paranEntry) {
        clexNode *pastEntry = entry;
        pastEntry->isStart = false;

        entry = makeNode(true, false);

        entry->transitions[0] = makeTransition('\0', '\0', pastEntry);
        clexNode *firstFinish =
            getFinishNode(pastEntry, state->getFinishNodeSeen);
        firstFinish->isFinish = false;

        clexNode *second = clexNfaFromRe(NULL, state);
        clexNode *secondFinish =
            getFinishNode(second, state->getFinishNodeSeen);
        secondFinish->isFinish = false;
        entry->transitions[1] = makeTransition('\0', '\0', second);

        clexNode *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        secondFinish->transitions[0] = makeTransition('\0', '\0', finish);

        last = finish;
      } else {
        clexNode *pipeEntry =
            makeNode(state->beforeParanEntry ? false : true, false);
        if (state->lastBeforeParanEntry) {
          state->lastBeforeParanEntry->transitions[0] = makeTransition(
              state->lastBeforeParanEntry->transitions[0]->fromValue,
              state->lastBeforeParanEntry->transitions[0]->toValue, pipeEntry);
          state->lastBeforeParanEntry = NULL;
        } else if (state->beforeParanEntry) {
          for (int i = 0; i < 100; i++)
            if (state->beforeParanEntry->transitions[i])
              state->beforeParanEntry->transitions[i] = makeTransition(
                  state->beforeParanEntry->transitions[i]->fromValue,
                  state->beforeParanEntry->transitions[i]->toValue, pipeEntry);
          state->beforeParanEntry = pipeEntry;
        } else {
          entry = pipeEntry;
          state->beforeParanEntry = entry;
        }
        pipeEntry->transitions[0] =
            makeTransition('\0', '\0', state->paranEntry);

        clexNode *firstFinish =
            getFinishNode(state->paranEntry, state->getFinishNodeSeen);
        firstFinish->isFinish = false;

        clexNode *second = clexNfaFromRe(NULL, state);
        clexNode *secondFinish =
            getFinishNode(second, state->getFinishNodeSeen);
        secondFinish->isFinish = false;
        pipeEntry->transitions[1] = makeTransition('\0', '\0', second);

        clexNode *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        secondFinish->transitions[0] = makeTransition('\0', '\0', finish);

        last = finish;
      }
    }
    if (token->kind == STAR) {
      if (!state->paranEntry) {
        clexNode *pastEntry = entry;
        pastEntry->isStart = false;

        clexNode *finish = makeNode(false, true);
        entry = makeNode(true, false);

        entry->transitions[0] = makeTransition('\0', '\0', pastEntry);
        entry->transitions[1] = makeTransition('\0', '\0', finish);
        clexNode *firstFinish =
            getFinishNode(pastEntry, state->getFinishNodeSeen);
        firstFinish->isFinish = false;
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        firstFinish->transitions[1] = makeTransition('\0', '\0', pastEntry);

        last = finish;
      } else {
        clexNode *starEntry =
            makeNode(state->beforeParanEntry ? false : true, false);
        if (state->lastBeforeParanEntry) {
          state->lastBeforeParanEntry->transitions[0] = makeTransition(
              state->lastBeforeParanEntry->transitions[0]->fromValue,
              state->lastBeforeParanEntry->transitions[0]->toValue, starEntry);
          state->lastBeforeParanEntry = NULL;
        } else if (state->beforeParanEntry)
          state->beforeParanEntry->transitions[0] = makeTransition(
              state->beforeParanEntry->transitions[0]->fromValue,
              state->beforeParanEntry->transitions[0]->toValue, starEntry);
        else
          entry = starEntry;

        clexNode *finish = makeNode(false, true);

        starEntry->transitions[0] =
            makeTransition('\0', '\0', state->paranEntry);
        starEntry->transitions[1] = makeTransition('\0', '\0', finish);
        clexNode *firstFinish =
            getFinishNode(state->paranEntry, state->getFinishNodeSeen);
        firstFinish->isFinish = false;
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        firstFinish->transitions[1] = makeTransition(
            '\0', '\0',
            state->beforeParanEntry && state->pipeSeen ? state->beforeParanEntry
                                                       : starEntry);

        last = finish;
      }
    }
    if (token->kind == PLUS) {
      clexNode *finish = getFinishNode(entry, state->getFinishNodeSeen);
      finish->transitions[1] = makeTransition(
          '\0', '\0',
          state->beforeParanEntry ? state->beforeParanEntry : entry);
    }
    if (token->kind == QUESTION) {
      if (!state->paranEntry) {
        clexNode *pastEntry = entry;
        pastEntry->isStart = false;

        entry = makeNode(true, false);

        entry->transitions[0] = makeTransition('\0', '\0', pastEntry);
        clexNode *firstFinish =
            getFinishNode(pastEntry, state->getFinishNodeSeen);
        firstFinish->isFinish = false;

        clexNode *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        entry->transitions[1] = makeTransition('\0', '\0', finish);

        last = finish;
      } else {
        clexNode *questionEntry =
            makeNode(state->beforeParanEntry ? false : true, false);
        if (state->lastBeforeParanEntry) {
          state->lastBeforeParanEntry->transitions[0] = makeTransition(
              state->lastBeforeParanEntry->transitions[0]->fromValue,
              state->lastBeforeParanEntry->transitions[0]->toValue,
              questionEntry);
          state->lastBeforeParanEntry = NULL;
        } else if (state->beforeParanEntry)
          state->beforeParanEntry->transitions[0] = makeTransition(
              state->beforeParanEntry->transitions[0]->fromValue,
              state->beforeParanEntry->transitions[0]->toValue, questionEntry);
        else
          entry = questionEntry;

        questionEntry->transitions[0] =
            makeTransition('\0', '\0', state->paranEntry);
        clexNode *firstFinish =
            getFinishNode(state->paranEntry, state->getFinishNodeSeen);
        firstFinish->isFinish = false;

        clexNode *finish = makeNode(false, true);
        firstFinish->transitions[0] = makeTransition('\0', '\0', finish);
        questionEntry->transitions[1] = makeTransition('\0', '\0', firstFinish);

        last = finish;
      }
    }
    if (token->kind == OSBRACKET) {
      int index = 0;
      clexNode *node = makeNode(false, true);
      int kind;
      while ((kind = peek(state)->kind) != CSBRACKET) {
        if (kind == EOF)
          return NULL;
        char fromValue = lex(state)->lexeme;
        if (peek(state)->kind == DASH) {
          lex(state);
          char toValue = lex(state)->lexeme;
          last->transitions[index++] = makeTransition(fromValue, toValue, node);
        } else {
          last->transitions[index++] =
              makeTransition(fromValue, fromValue, node);
        }
      }
      lex(state);
      if (peek(state)->kind == OPARAN) {
        state->lastBeforeParanEntry = state->beforeParanEntry;
        state->beforeParanEntry = last;
      }
      last->isFinish = false;
      last = node;
    }
  }
  return entry;
}

bool clexNfaTest(clexNode *nfa, const char *target) {
  for (size_t i = 0; i < strlen(target); i++) {
    for (int j = 0; j < 100; j++)
      if (nfa->transitions[j]) {
        if (nfa->transitions[j]->fromValue <= target[i] &&
            nfa->transitions[j]->toValue >= target[i]) {
          if (clexNfaTest(nfa->transitions[j]->to, target + i + 1))
            return true;
        } else if (nfa->transitions[j]->fromValue == '\0') {
          if (clexNfaTest(nfa->transitions[j]->to, target + i))
            return true;
        }
      }
    return false;
  }
  for (int j = 0; j < 100; j++) {
    if (nfa->transitions[j] && nfa->transitions[j]->fromValue == '\0')
      if (clexNfaTest(nfa->transitions[j]->to, ""))
        return true;
  }
  if (nfa->isFinish)
    return true;
  return false;
}

static char *drawKey(clexNode *node1, clexNode *node2, char fromValue,
                     char toValue) {
  char *result = malloc(1024);
  sprintf(result, "%p%p%c%c", node1, node2, fromValue, toValue);
  return result;
}

static unsigned long getDrawMapping(unsigned long *drawMapping,
                                    unsigned long value) {
  for (int i = 0; i < 1024; i++)
    if (drawMapping[i] == value)
      return i;
    else if (drawMapping[i] == 0) {
      drawMapping[i] = value;
      return i;
    }
}

static void drawNode(clexNode *nfa, char **drawSeen,
                     unsigned long *drawMapping) {
  for (int i = 0; i < 100; i++)
    if (nfa->transitions[i] &&
        !inArray(drawSeen, drawKey(nfa, nfa->transitions[i]->to,
                                   nfa->transitions[i]->fromValue,
                                   nfa->transitions[i]->toValue))) {
      if (nfa->transitions[i]->fromValue || nfa->transitions[i]->toValue)
        printf(
            "  %lu -> %lu [label=\"%c-%c\"];\n",
            getDrawMapping(drawMapping, (unsigned long)nfa),
            getDrawMapping(drawMapping, (unsigned long)nfa->transitions[i]->to),
            nfa->transitions[i]->fromValue ? nfa->transitions[i]->fromValue
                                           : ' ',
            nfa->transitions[i]->toValue ? nfa->transitions[i]->toValue : ' ');
      else
        printf("  %lu -> %lu [label=\"e\"];\n",
               getDrawMapping(drawMapping, (unsigned long)nfa),
               getDrawMapping(drawMapping,
                              (unsigned long)nfa->transitions[i]->to));
      if (!inArray(drawSeen, drawKey(nfa, nfa->transitions[i]->to,
                                     nfa->transitions[i]->fromValue,
                                     nfa->transitions[i]->toValue))) {
        insertArray(drawSeen, drawKey(nfa, nfa->transitions[i]->to,
                                      nfa->transitions[i]->fromValue,
                                      nfa->transitions[i]->toValue));
        drawNode(nfa->transitions[i]->to, drawSeen, drawMapping);
      }
    }
}

void clexNfaDraw(clexNode *nfa) {
  char **drawSeen = calloc(1024, sizeof(char *));
  unsigned long *drawMapping = calloc(1024, sizeof(unsigned long));
  printf("digraph G {\n");
  drawNode(nfa, drawSeen, drawMapping);
  printf("}\n");
}
