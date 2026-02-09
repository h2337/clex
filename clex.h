#ifndef CLEX_H
#define CLEX_H

#include <stdbool.h>
#include <stddef.h>

#include "fa.h"

#define CLEX_MAX_RULES 1024
#define CLEX_TOKEN_EOF (-1)
#define CLEX_TOKEN_ERROR (-2)

typedef enum clexStatus {
  CLEX_STATUS_OK = 0,
  CLEX_STATUS_EOF,
  CLEX_STATUS_INVALID_ARGUMENT,
  CLEX_STATUS_OUT_OF_MEMORY,
  CLEX_STATUS_REGEX_ERROR,
  CLEX_STATUS_RULE_LIMIT_REACHED,
  CLEX_STATUS_NO_RULES,
  CLEX_STATUS_LEXICAL_ERROR
} clexStatus;

typedef struct clexSourcePosition {
  size_t offset;
  size_t line;
  size_t column;
} clexSourcePosition;

typedef struct clexSourceSpan {
  clexSourcePosition start;
  clexSourcePosition end;
} clexSourceSpan;

typedef struct clexRule {
  const char* re;
  clexNode* nfa;
  int kind;
} clexRule;

typedef struct clexToken {
  int kind;
  char* lexeme;
  clexSourceSpan span;
} clexToken;

typedef struct clexError {
  clexStatus status;
  clexSourcePosition position;
  char* offending_lexeme;
  int* expected_kinds;
  size_t expected_kind_count;
} clexError;

typedef struct clexLexer {
  clexRule** rules;
  const char* content;
  size_t position;
  size_t line;
  size_t column;
  clexError last_error;
} clexLexer;

clexLexer* clexInit(void);
void clexLexerDestroy(clexLexer* lexer);
void clexReset(clexLexer* lexer, const char* content);
void clexTokenInit(clexToken* token);
void clexTokenClear(clexToken* token);
void clexErrorInit(clexError* error);
void clexErrorClear(clexError* error);
const clexError* clexGetLastError(const clexLexer* lexer);
clexStatus clexRegisterKind(clexLexer* lexer, const char* re, int kind);
void clexDeleteKinds(clexLexer* lexer);
clexStatus clex(clexLexer* lexer, clexToken* out_token);

#endif
