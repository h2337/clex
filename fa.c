#include "fa.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef EOF

static clexNode* makeNode(bool isStart, bool isFinish) {
  clexNode* result = malloc(sizeof(clexNode));
  if (!result) return NULL;
  result->isStart = isStart;
  result->isFinish = isFinish;
  result->transitions = NULL;
  result->transitionCount = 0;
  result->transitionCapacity = 0;
  result->compiled = NULL;
  return result;
}

static clexTransition* makeTransition(char fromValue, char toValue,
                                      clexNode* to) {
  clexTransition* result = malloc(sizeof(clexTransition));
  if (!result) return NULL;
  result->fromValue = fromValue;
  result->toValue = toValue;
  result->to = to;
  return result;
}

static bool ensureNodeTransitionCapacity(clexNode* node, size_t required) {
  if (!node) return false;
  if (node->transitionCapacity >= required) return true;

  size_t newCapacity = node->transitionCapacity ? node->transitionCapacity : 4;
  while (newCapacity < required) {
    if (newCapacity > (size_t)-1 / 2) {
      newCapacity = required;
      break;
    }
    newCapacity *= 2;
  }

  clexTransition** resized =
      realloc(node->transitions, newCapacity * sizeof(clexTransition*));
  if (!resized) return false;
  memset(resized + node->transitionCapacity, 0,
         (newCapacity - node->transitionCapacity) * sizeof(clexTransition*));
  node->transitions = resized;
  node->transitionCapacity = newCapacity;
  return true;
}

static bool nodeSetTransition(clexNode* node, size_t index,
                              clexTransition* transition) {
  if (!node || !transition) return false;
  if (!ensureNodeTransitionCapacity(node, index + 1)) return false;

  if (node->transitions[index] && node->transitions[index] != transition)
    free(node->transitions[index]);
  node->transitions[index] = transition;
  if (node->transitionCount < index + 1) node->transitionCount = index + 1;
  return true;
}

static bool nodeSetTransitionValues(clexNode* node, size_t index,
                                    char fromValue, char toValue,
                                    clexNode* to) {
  clexTransition* transition = makeTransition(fromValue, toValue, to);
  if (!transition) return false;
  if (!nodeSetTransition(node, index, transition)) {
    free(transition);
    return false;
  }
  return true;
}

static bool nodeRepointTransition(clexNode* node, size_t index, clexNode* to) {
  if (!node || index >= node->transitionCount || !node->transitions[index])
    return false;
  clexTransition* current = node->transitions[index];
  return nodeSetTransitionValues(node, index, current->fromValue,
                                 current->toValue, to);
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

static Token* makeToken(TokenKind kind) {
  Token* result = malloc(sizeof(Token));
  if (!result) return NULL;
  result->kind = kind;
  return result;
}

static Token* makeLexemeToken(TokenKind kind, char lexeme) {
  Token* result = malloc(sizeof(Token));
  if (!result) return NULL;
  result->kind = kind;
  result->lexeme = lexeme;
  return result;
}

static Token* lex(clexReLexerState* state) {
  switch (state->lexerContent[state->lexerPosition]) {
    case '\0':
      return makeToken(EOF);
    case '(':;
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
  Token* result =
      makeLexemeToken(LITERAL, state->lexerContent[state->lexerPosition]);
  state->lexerPosition++;
  return result;
}

static Token* peek(clexReLexerState* state) {
  Token* lexed = lex(state);
  if (!lexed) return NULL;
  if (lexed->kind != EOF) state->lexerPosition--;
  return lexed;
}

static bool inArray(char** array, char* key) {
  for (int i = 0; i < 1024; i++)
    if (array[i] && strcmp(array[i], key) == 0) return true;
  return false;
}

static void insertArray(char** array, char* key) {
  for (int i = 0; i < 1024; i++)
    if (!array[i]) {
      array[i] = key;
      return;
    }
}

typedef struct NodeVec {
  clexNode** items;
  size_t size;
  size_t capacity;
} NodeVec;

static void nodeVecFree(NodeVec* vec) {
  free(vec->items);
  vec->items = NULL;
  vec->size = 0;
  vec->capacity = 0;
}

static bool nodeVecContains(const NodeVec* vec, const clexNode* node) {
  if (!vec) return false;
  for (size_t i = 0; i < vec->size; i++)
    if (vec->items[i] == node) return true;
  return false;
}

static bool nodeVecIndexOf(const NodeVec* vec, const clexNode* node,
                           size_t* outIndex) {
  if (!vec || !outIndex) return false;
  for (size_t i = 0; i < vec->size; i++)
    if (vec->items[i] == node) {
      *outIndex = i;
      return true;
    }
  return false;
}

static bool nodeVecPush(NodeVec* vec, clexNode* node) {
  if (!vec) return false;
  if (vec->size == vec->capacity) {
    size_t newCapacity = vec->capacity ? vec->capacity * 2 : 64;
    clexNode** newItems = realloc(vec->items, newCapacity * sizeof(clexNode*));
    if (!newItems) return false;
    vec->items = newItems;
    vec->capacity = newCapacity;
  }
  vec->items[vec->size++] = node;
  return true;
}

typedef struct clexCompiledTransition {
  char fromValue;
  char toValue;
  size_t toIndex;
} clexCompiledTransition;

typedef struct clexCompiledNode {
  bool isFinish;
  clexCompiledTransition* transitions;
  size_t transitionCount;
} clexCompiledNode;

struct clexCompiledNfa {
  clexCompiledNode* nodes;
  size_t nodeCount;
  unsigned char* activeStates;
  unsigned char* seedStates;
  unsigned char* nextSeedStates;
  size_t* stack;
};

static void compiledNfaFree(clexCompiledNfa* compiled) {
  if (!compiled) return;
  if (compiled->nodes) {
    for (size_t i = 0; i < compiled->nodeCount; i++)
      free(compiled->nodes[i].transitions);
  }
  free(compiled->nodes);
  free(compiled->activeStates);
  free(compiled->seedStates);
  free(compiled->nextSeedStates);
  free(compiled->stack);
  compiled->nodes = NULL;
  compiled->activeStates = NULL;
  compiled->seedStates = NULL;
  compiled->nextSeedStates = NULL;
  compiled->stack = NULL;
  compiled->nodeCount = 0;
}

static bool collectReachableNodes(clexNode* start, NodeVec* nodes) {
  if (!start || !nodes) return false;
  if (!nodeVecPush(nodes, start)) return false;

  for (size_t i = 0; i < nodes->size; i++) {
    clexNode* node = nodes->items[i];
    for (size_t j = 0; j < node->transitionCount; j++) {
      if (!node->transitions[j] || !node->transitions[j]->to) continue;
      if (nodeVecContains(nodes, node->transitions[j]->to)) continue;
      if (!nodeVecPush(nodes, node->transitions[j]->to)) return false;
    }
  }
  return true;
}

static size_t nodeTransitionCount(const clexNode* node) {
  if (!node) return 0;
  size_t count = 0;
  for (size_t i = 0; i < node->transitionCount; i++)
    if (node->transitions[i]) count++;
  return count;
}

static void freeCompiledNodesArray(clexCompiledNode* nodes, size_t nodeCount) {
  if (!nodes) return;
  for (size_t i = 0; i < nodeCount; i++) free(nodes[i].transitions);
  free(nodes);
}

static bool buildCompiledNfa(clexNode* start, clexCompiledNfa* outCompiled) {
  if (!start || !outCompiled) return false;

  NodeVec nodes = {0};
  if (!collectReachableNodes(start, &nodes)) {
    nodeVecFree(&nodes);
    return false;
  }

  clexCompiledNode* compiledNodes =
      calloc(nodes.size, sizeof(clexCompiledNode));
  if (!compiledNodes) {
    nodeVecFree(&nodes);
    return false;
  }

  for (size_t i = 0; i < nodes.size; i++) {
    clexNode* node = nodes.items[i];
    compiledNodes[i].isFinish = node->isFinish;

    compiledNodes[i].transitionCount = nodeTransitionCount(node);
    if (compiledNodes[i].transitionCount == 0) continue;

    compiledNodes[i].transitions = calloc(compiledNodes[i].transitionCount,
                                          sizeof(clexCompiledTransition));
    if (!compiledNodes[i].transitions) {
      freeCompiledNodesArray(compiledNodes, nodes.size);
      nodeVecFree(&nodes);
      return false;
    }

    size_t transitionIndex = 0;
    for (size_t j = 0; j < node->transitionCount; j++) {
      if (!node->transitions[j]) continue;
      size_t toIndex = 0;
      if (!nodeVecIndexOf(&nodes, node->transitions[j]->to, &toIndex)) {
        freeCompiledNodesArray(compiledNodes, nodes.size);
        nodeVecFree(&nodes);
        return false;
      }

      compiledNodes[i].transitions[transitionIndex].fromValue =
          node->transitions[j]->fromValue;
      compiledNodes[i].transitions[transitionIndex].toValue =
          node->transitions[j]->toValue;
      compiledNodes[i].transitions[transitionIndex].toIndex = toIndex;
      transitionIndex++;
    }
  }

  outCompiled->nodes = compiledNodes;
  outCompiled->nodeCount = nodes.size;
  outCompiled->activeStates = calloc(nodes.size, sizeof(unsigned char));
  outCompiled->seedStates = calloc(nodes.size, sizeof(unsigned char));
  outCompiled->nextSeedStates = calloc(nodes.size, sizeof(unsigned char));
  outCompiled->stack = calloc(nodes.size, sizeof(size_t));
  if (!outCompiled->activeStates || !outCompiled->seedStates ||
      !outCompiled->nextSeedStates || !outCompiled->stack) {
    compiledNfaFree(outCompiled);
    nodeVecFree(&nodes);
    return false;
  }
  nodeVecFree(&nodes);
  return true;
}

static bool stateSetHasAny(const unsigned char* states, size_t stateCount) {
  for (size_t i = 0; i < stateCount; i++)
    if (states[i]) return true;
  return false;
}

static void epsilonClosure(const clexCompiledNfa* compiled,
                           const unsigned char* seedStates,
                           unsigned char* outStates, size_t* stack) {
  size_t stackSize = 0;
  memset(outStates, 0, compiled->nodeCount * sizeof(unsigned char));

  for (size_t i = 0; i < compiled->nodeCount; i++) {
    if (!seedStates[i]) continue;
    outStates[i] = 1;
    stack[stackSize++] = i;
  }

  while (stackSize > 0) {
    size_t index = stack[--stackSize];
    clexCompiledNode* node = &compiled->nodes[index];
    for (size_t i = 0; i < node->transitionCount; i++) {
      clexCompiledTransition* transition = &node->transitions[i];
      if (transition->fromValue != '\0') continue;
      if (outStates[transition->toIndex]) continue;
      outStates[transition->toIndex] = 1;
      stack[stackSize++] = transition->toIndex;
    }
  }
}

static bool runCompiledNfa(const clexCompiledNfa* compiled,
                           const char* target) {
  if (!compiled || !target || compiled->nodeCount == 0) return false;
  if (!compiled->activeStates || !compiled->seedStates ||
      !compiled->nextSeedStates || !compiled->stack)
    return false;

  bool matched = false;

  memset(compiled->seedStates, 0, compiled->nodeCount * sizeof(unsigned char));
  compiled->seedStates[0] = 1;
  epsilonClosure(compiled, compiled->seedStates, compiled->activeStates,
                 compiled->stack);

  for (size_t i = 0; target[i] != '\0'; i++) {
    char symbol = target[i];
    memset(compiled->nextSeedStates, 0,
           compiled->nodeCount * sizeof(unsigned char));

    for (size_t j = 0; j < compiled->nodeCount; j++) {
      if (!compiled->activeStates[j]) continue;
      clexCompiledNode* node = &compiled->nodes[j];

      for (size_t k = 0; k < node->transitionCount; k++) {
        clexCompiledTransition* transition = &node->transitions[k];
        if (transition->fromValue == '\0') continue;
        if (transition->fromValue <= symbol && transition->toValue >= symbol)
          compiled->nextSeedStates[transition->toIndex] = 1;
      }
    }

    if (!stateSetHasAny(compiled->nextSeedStates, compiled->nodeCount))
      return false;
    epsilonClosure(compiled, compiled->nextSeedStates, compiled->activeStates,
                   compiled->stack);
    if (!stateSetHasAny(compiled->activeStates, compiled->nodeCount))
      return false;
  }

  for (size_t i = 0; i < compiled->nodeCount; i++)
    if (compiled->activeStates[i] && compiled->nodes[i].isFinish) {
      matched = true;
      break;
    }

  return matched;
}

static clexNode* getFinishNodeInternal(clexNode* node, NodeVec* seen) {
  if (!node) return NULL;
  if (node->isFinish) return node;
  if (nodeVecContains(seen, node)) return NULL;
  if (!nodeVecPush(seen, node)) return NULL;

  for (size_t i = 0; i < node->transitionCount; i++) {
    if (!node->transitions[i]) continue;
    clexNode* result = getFinishNodeInternal(node->transitions[i]->to, seen);
    if (result) return result;
  }
  return NULL;
}

static clexNode* getFinishNode(clexNode* node) {
  NodeVec seen = {0};
  clexNode* result = getFinishNodeInternal(node, &seen);
  nodeVecFree(&seen);
  return result;
}

static bool validateRegexSyntax(const char* re) {
  int parenDepth = 0;
  bool inCharClass = false;
  bool escaped = false;
  bool charClassHasContent = false;
  bool hasAtom = false;
  bool lastWasPipe = false;
  bool lastWasQuantifier = false;

  for (size_t i = 0; re[i] != '\0'; i++) {
    const char c = re[i];
    if (escaped) {
      escaped = false;
      hasAtom = true;
      lastWasPipe = false;
      lastWasQuantifier = false;
      continue;
    }
    if (c == '\\') {
      escaped = true;
      continue;
    }
    if (inCharClass) {
      if (c == ']') {
        if (!charClassHasContent) return false;
        inCharClass = false;
        hasAtom = true;
        lastWasPipe = false;
        lastWasQuantifier = false;
      } else {
        charClassHasContent = true;
      }
      continue;
    }

    if (c == '[') {
      inCharClass = true;
      charClassHasContent = false;
      continue;
    }
    if (c == ']') return false;

    if (c == '(') {
      parenDepth++;
      hasAtom = false;
      lastWasPipe = false;
      lastWasQuantifier = false;
      continue;
    }
    if (c == ')') {
      if (parenDepth == 0 || !hasAtom) return false;
      parenDepth--;
      hasAtom = true;
      lastWasPipe = false;
      lastWasQuantifier = false;
      continue;
    }

    if (c == '|') {
      if (!hasAtom) return false;
      hasAtom = false;
      lastWasPipe = true;
      lastWasQuantifier = false;
      continue;
    }

    if (c == '*' || c == '+' || c == '?') {
      if (!hasAtom || lastWasQuantifier) return false;
      lastWasPipe = false;
      lastWasQuantifier = true;
      continue;
    }

    hasAtom = true;
    lastWasPipe = false;
    lastWasQuantifier = false;
  }

  if (escaped || inCharClass || parenDepth != 0 || lastWasPipe) return false;
  return true;
}

clexNode* clexNfaFromRe(const char* re, clexReLexerState* state) {
  bool isOuter = false;
  if (!state) {
    state = calloc(1, sizeof(clexReLexerState));
    if (!state) return NULL;
    isOuter = true;
  }
  if (re) {
    if (!validateRegexSyntax(re)) {
      if (isOuter) free(state);
      return NULL;
    }
    state->lexerContent = re;
    state->lexerPosition = 0;
    state->lastBeforeParanEntry = NULL;
    state->beforeParanEntry = NULL;
    state->paranEntry = NULL;
    state->inPipe = false;
    state->pipeSeen = false;
    state->inBackslash = false;
  }

  Token* token;
  clexNode* entry = makeNode(true, true);
  if (!entry) {
    if (isOuter) free(state);
    return NULL;
  }
  clexNode* last = entry;
  while ((token = lex(state)) && token->kind != EOF) {
    if (state->inBackslash) {
      state->inBackslash = false;
      clexNode* node = makeNode(false, true);
      if (!node) {
        free(token);
        clexNfaDestroy(entry, NULL);
        if (isOuter) free(state);
        return NULL;
      }
      nodeSetTransitionValues(last, 0, token->lexeme, token->lexeme, node);
      last->isFinish = false;
      last = node;
      free(token);
      continue;
    }
    Token* peeked = peek(state);
    if (!peeked) {
      free(token);
      clexNfaDestroy(entry, NULL);
      if (isOuter) free(state);
      return NULL;
    }
    if (peeked->kind == OPARAN) {
      state->lastBeforeParanEntry = state->beforeParanEntry;
      state->beforeParanEntry = last;
    }
    free(peeked);
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
        free(token);
        return entry;
      }
    }
    if (token->kind == LITERAL) {
      clexNode* node = makeNode(false, true);
      if (!node) {
        free(token);
        clexNfaDestroy(entry, NULL);
        if (isOuter) free(state);
        return NULL;
      }
      nodeSetTransitionValues(last, 0, token->lexeme, token->lexeme, node);
      last->isFinish = false;
      last = node;
    }
    if (token->kind == PIPE) {
      state->inPipe = true;
      if (!state->paranEntry) {
        clexNode* pastEntry = entry;
        pastEntry->isStart = false;

        entry = makeNode(true, false);
        if (!entry) {
          free(token);
          clexNfaDestroy(pastEntry, NULL);
          if (isOuter) free(state);
          return NULL;
        }

        nodeSetTransitionValues(entry, 0, '\0', '\0', pastEntry);
        clexNode* firstFinish = getFinishNode(pastEntry);
        if (!firstFinish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        firstFinish->isFinish = false;

        clexNode* second = clexNfaFromRe(NULL, state);
        clexNode* secondFinish = getFinishNode(second);
        if (!second || !secondFinish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        secondFinish->isFinish = false;
        nodeSetTransitionValues(entry, 1, '\0', '\0', second);

        clexNode* finish = makeNode(false, true);
        if (!finish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        nodeSetTransitionValues(firstFinish, 0, '\0', '\0', finish);
        nodeSetTransitionValues(secondFinish, 0, '\0', '\0', finish);

        last = finish;
      } else {
        clexNode* pipeEntry =
            makeNode(state->beforeParanEntry ? false : true, false);
        if (state->lastBeforeParanEntry) {
          nodeRepointTransition(state->lastBeforeParanEntry, 0, pipeEntry);
          state->lastBeforeParanEntry = NULL;
        } else if (state->beforeParanEntry) {
          for (size_t i = 0; i < state->beforeParanEntry->transitionCount; i++)
            if (state->beforeParanEntry->transitions[i])
              nodeRepointTransition(state->beforeParanEntry, i, pipeEntry);
          state->beforeParanEntry = pipeEntry;
        } else {
          entry = pipeEntry;
          state->beforeParanEntry = entry;
        }
        nodeSetTransitionValues(pipeEntry, 0, '\0', '\0', state->paranEntry);

        clexNode* firstFinish = getFinishNode(state->paranEntry);
        if (!firstFinish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        firstFinish->isFinish = false;

        clexNode* second = clexNfaFromRe(NULL, state);
        clexNode* secondFinish = getFinishNode(second);
        if (!second || !secondFinish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        secondFinish->isFinish = false;
        nodeSetTransitionValues(pipeEntry, 1, '\0', '\0', second);

        clexNode* finish = makeNode(false, true);
        if (!finish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        nodeSetTransitionValues(firstFinish, 0, '\0', '\0', finish);
        nodeSetTransitionValues(secondFinish, 0, '\0', '\0', finish);

        last = finish;
      }
    }
    if (token->kind == STAR) {
      if (!state->paranEntry) {
        clexNode* pastEntry = entry;
        pastEntry->isStart = false;

        clexNode* finish = makeNode(false, true);
        entry = makeNode(true, false);
        if (!entry || !finish) {
          free(token);
          clexNfaDestroy(pastEntry, NULL);
          if (entry) clexNfaDestroy(entry, NULL);
          if (finish) clexNfaDestroy(finish, NULL);
          if (isOuter) free(state);
          return NULL;
        }

        nodeSetTransitionValues(entry, 0, '\0', '\0', pastEntry);
        nodeSetTransitionValues(entry, 1, '\0', '\0', finish);
        clexNode* firstFinish = getFinishNode(pastEntry);
        if (!firstFinish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        firstFinish->isFinish = false;
        nodeSetTransitionValues(firstFinish, 0, '\0', '\0', finish);
        nodeSetTransitionValues(firstFinish, 1, '\0', '\0', pastEntry);

        last = finish;
      } else {
        clexNode* starEntry =
            makeNode(state->beforeParanEntry ? false : true, false);
        if (state->lastBeforeParanEntry) {
          nodeRepointTransition(state->lastBeforeParanEntry, 0, starEntry);
          state->lastBeforeParanEntry = NULL;
        } else if (state->beforeParanEntry)
          nodeRepointTransition(state->beforeParanEntry, 0, starEntry);
        else
          entry = starEntry;

        clexNode* finish = makeNode(false, true);
        if (!finish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }

        nodeSetTransitionValues(starEntry, 0, '\0', '\0', state->paranEntry);
        nodeSetTransitionValues(starEntry, 1, '\0', '\0', finish);
        clexNode* firstFinish = getFinishNode(state->paranEntry);
        if (!firstFinish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        firstFinish->isFinish = false;
        nodeSetTransitionValues(firstFinish, 0, '\0', '\0', finish);
        nodeSetTransitionValues(firstFinish, 1, '\0', '\0',
                                state->beforeParanEntry && state->pipeSeen
                                    ? state->beforeParanEntry
                                    : starEntry);

        last = finish;
      }
    }
    if (token->kind == PLUS) {
      clexNode* finish = getFinishNode(entry);
      if (!finish) {
        free(token);
        clexNfaDestroy(entry, NULL);
        if (isOuter) free(state);
        return NULL;
      }
      nodeSetTransitionValues(
          finish, 1, '\0', '\0',
          state->beforeParanEntry ? state->beforeParanEntry : entry);
    }
    if (token->kind == QUESTION) {
      if (!state->paranEntry) {
        clexNode* pastEntry = entry;
        pastEntry->isStart = false;

        entry = makeNode(true, false);
        if (!entry) {
          free(token);
          clexNfaDestroy(pastEntry, NULL);
          if (isOuter) free(state);
          return NULL;
        }

        nodeSetTransitionValues(entry, 0, '\0', '\0', pastEntry);
        clexNode* firstFinish = getFinishNode(pastEntry);
        if (!firstFinish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        firstFinish->isFinish = false;

        clexNode* finish = makeNode(false, true);
        if (!finish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        nodeSetTransitionValues(firstFinish, 0, '\0', '\0', finish);
        nodeSetTransitionValues(entry, 1, '\0', '\0', finish);

        last = finish;
      } else {
        clexNode* questionEntry =
            makeNode(state->beforeParanEntry ? false : true, false);
        if (state->lastBeforeParanEntry) {
          nodeRepointTransition(state->lastBeforeParanEntry, 0, questionEntry);
          state->lastBeforeParanEntry = NULL;
        } else if (state->beforeParanEntry)
          nodeRepointTransition(state->beforeParanEntry, 0, questionEntry);
        else
          entry = questionEntry;

        nodeSetTransitionValues(questionEntry, 0, '\0', '\0',
                                state->paranEntry);
        clexNode* firstFinish = getFinishNode(state->paranEntry);
        if (!firstFinish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        firstFinish->isFinish = false;

        clexNode* finish = makeNode(false, true);
        if (!finish) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        nodeSetTransitionValues(firstFinish, 0, '\0', '\0', finish);
        nodeSetTransitionValues(questionEntry, 1, '\0', '\0', firstFinish);

        last = finish;
      }
    }
    if (token->kind == OSBRACKET) {
      size_t index = 0;
      clexNode* node = makeNode(false, true);
      if (!node) {
        free(token);
        clexNfaDestroy(entry, NULL);
        if (isOuter) free(state);
        return NULL;
      }
      int kind;
      Token* lexed;
      while (true) {
        lexed = peek(state);
        if (!lexed) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        kind = lexed->kind;
        if (kind == CSBRACKET) break;
        free(lexed);
        if (kind == EOF) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        lexed = lex(state);
        if (!lexed) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        char fromValue = lexed->lexeme;
        free(lexed);
        Token* peeked = peek(state);
        if (!peeked) {
          free(token);
          clexNfaDestroy(entry, NULL);
          if (isOuter) free(state);
          return NULL;
        }
        if (peeked->kind == DASH) {
          Token* dash = peek(state);
          free(dash);
          lexed = lex(state);
          if (!lexed) {
            free(peeked);
            free(token);
            clexNfaDestroy(entry, NULL);
            if (isOuter) free(state);
            return NULL;
          }
          free(lexed);
          lexed = lex(state);
          if (!lexed) {
            free(peeked);
            free(token);
            clexNfaDestroy(entry, NULL);
            if (isOuter) free(state);
            return NULL;
          }
          char toValue = lexed->lexeme;
          free(lexed);
          nodeSetTransitionValues(last, index++, fromValue, toValue, node);
        } else {
          nodeSetTransitionValues(last, index++, fromValue, fromValue, node);
        }
        free(peeked);
      }
      free(lexed);
      lexed = lex(state);
      if (!lexed) {
        free(token);
        clexNfaDestroy(entry, NULL);
        if (isOuter) free(state);
        return NULL;
      }
      free(lexed);
      peeked = peek(state);
      if (!peeked) {
        free(token);
        clexNfaDestroy(entry, NULL);
        if (isOuter) free(state);
        return NULL;
      }
      if (peeked->kind == OPARAN) {
        state->lastBeforeParanEntry = state->beforeParanEntry;
        state->beforeParanEntry = last;
      }
      free(peeked);
      last->isFinish = false;
      last = node;
    }
    free(token);
  }
  if (!token) {
    clexNfaDestroy(entry, NULL);
    if (isOuter) free(state);
    return NULL;
  }
  free(token);
  if (isOuter) free(state);
  return entry;
}

bool clexNfaTest(clexNode* nfa, const char* target) {
  if (!nfa || !target) return false;

  if (!nfa->compiled) {
    nfa->compiled = calloc(1, sizeof(clexCompiledNfa));
    if (!nfa->compiled) return false;
    if (!buildCompiledNfa(nfa, nfa->compiled)) {
      free(nfa->compiled);
      nfa->compiled = NULL;
      return false;
    }
  }

  return runCompiledNfa(nfa->compiled, target);
}

static char* drawKey(clexNode* node1, clexNode* node2, char fromValue,
                     char toValue) {
  char* result = malloc(1024);
  sprintf(result, "%p%p%c%c", (void*)node1, (void*)node2, fromValue, toValue);
  return result;
}

static unsigned long getDrawMapping(unsigned long* drawMapping,
                                    unsigned long value) {
  for (int i = 0; i < 1024; i++)
    if (drawMapping[i] == value)
      return i;
    else if (drawMapping[i] == 0) {
      drawMapping[i] = value;
      return i;
    }
  return -1;
}

static void drawNode(clexNode* nfa, char** drawSeen,
                     unsigned long* drawMapping) {
  for (size_t i = 0; i < nfa->transitionCount; i++) {
    if (nfa->transitions[i]) {
      char* key =
          drawKey(nfa, nfa->transitions[i]->to, nfa->transitions[i]->fromValue,
                  nfa->transitions[i]->toValue);
      if (!inArray(drawSeen, key)) {
        if (nfa->transitions[i]->fromValue || nfa->transitions[i]->toValue)
          printf("  %lu -> %lu [label=\"%c-%c\"];\n",
                 getDrawMapping(drawMapping, (unsigned long)nfa),
                 getDrawMapping(drawMapping,
                                (unsigned long)nfa->transitions[i]->to),
                 nfa->transitions[i]->fromValue ? nfa->transitions[i]->fromValue
                                                : ' ',
                 nfa->transitions[i]->toValue ? nfa->transitions[i]->toValue
                                              : ' ');
        else
          printf("  %lu -> %lu [label=\"e\"];\n",
                 getDrawMapping(drawMapping, (unsigned long)nfa),
                 getDrawMapping(drawMapping,
                                (unsigned long)nfa->transitions[i]->to));
        insertArray(drawSeen, key);
        drawNode(nfa->transitions[i]->to, drawSeen, drawMapping);
      } else {
        free(key);
      }
    }
  }
}

void clexNfaDraw(clexNode* nfa) {
  char** drawSeen = calloc(1024, sizeof(char*));
  unsigned long* drawMapping = calloc(1024, sizeof(unsigned long));
  printf("digraph G {\n");
  drawNode(nfa, drawSeen, drawMapping);
  printf("}\n");
  for (int i = 0; i < 1024; i++) free(drawSeen[i]);
  free(drawSeen);
  free(drawMapping);
}

static void clexNfaDestroyInternal(clexNode* nfa, NodeVec* seen) {
  if (!nfa || nodeVecContains(seen, nfa)) return;
  if (!nodeVecPush(seen, nfa)) return;

  for (size_t i = 0; i < nfa->transitionCount; i++) {
    if (!nfa->transitions[i]) continue;
    clexNfaDestroyInternal(nfa->transitions[i]->to, seen);
    free(nfa->transitions[i]);
  }
  if (nfa->compiled) {
    compiledNfaFree(nfa->compiled);
    free(nfa->compiled);
  }
  free(nfa->transitions);
  free(nfa);
}

void clexNfaDestroy(clexNode* nfa, clexNode** seen) {
  (void)seen;
  NodeVec visited = {0};
  clexNfaDestroyInternal(nfa, &visited);
  nodeVecFree(&visited);
}
