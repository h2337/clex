#define _CRT_SECURE_NO_WARNINGS
#include "clex.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "fa.h"

static clexSourcePosition make_position(size_t offset, size_t line,
                                        size_t column) {
  clexSourcePosition position;
  position.offset = offset;
  position.line = line;
  position.column = column;
  return position;
}

static clexSourcePosition advance_position(clexSourcePosition position,
                                           const char* text, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    if (text[i] == '\n') {
      position.line++;
      position.column = 1;
    } else {
      position.column++;
    }
    position.offset++;
  }
  return position;
}

void clexTokenInit(clexToken* token) {
  if (!token) return;
  token->kind = CLEX_TOKEN_EOF;
  token->lexeme = NULL;
  token->span.start = make_position(0, 1, 1);
  token->span.end = make_position(0, 1, 1);
}

void clexTokenClear(clexToken* token) {
  if (!token) return;
  free(token->lexeme);
  token->lexeme = NULL;
}

void clexErrorInit(clexError* error) {
  if (!error) return;
  error->status = CLEX_STATUS_OK;
  error->position = make_position(0, 1, 1);
  error->offending_lexeme = NULL;
  error->expected_kinds = NULL;
  error->expected_kind_count = 0;
}

void clexErrorClear(clexError* error) {
  if (!error) return;
  free(error->offending_lexeme);
  error->offending_lexeme = NULL;
  free(error->expected_kinds);
  error->expected_kinds = NULL;
  error->expected_kind_count = 0;
  error->status = CLEX_STATUS_OK;
  error->position = make_position(0, 1, 1);
}

static clexStatus lexer_set_error(clexLexer* lexer, clexStatus status,
                                  clexSourcePosition position,
                                  const char* offending_lexeme) {
  if (!lexer) return status;
  clexErrorClear(&lexer->last_error);
  lexer->last_error.status = status;
  lexer->last_error.position = position;
  if (offending_lexeme) {
    size_t length = strlen(offending_lexeme);
    lexer->last_error.offending_lexeme = calloc(length + 1, sizeof(char));
    if (!lexer->last_error.offending_lexeme) {
      lexer->last_error.status = CLEX_STATUS_OUT_OF_MEMORY;
      return CLEX_STATUS_OUT_OF_MEMORY;
    }
    memcpy(lexer->last_error.offending_lexeme, offending_lexeme, length);
  }
  return status;
}

static bool add_expected_kind_unique(clexError* error, int kind) {
  for (size_t i = 0; i < error->expected_kind_count; ++i) {
    if (error->expected_kinds[i] == kind) {
      return true;
    }
  }
  int* updated = realloc(error->expected_kinds,
                         (error->expected_kind_count + 1) * sizeof(int));
  if (!updated) {
    return false;
  }
  error->expected_kinds = updated;
  error->expected_kinds[error->expected_kind_count++] = kind;
  return true;
}

static clexStatus lexer_fill_expected_kinds(clexLexer* lexer) {
  if (!lexer || !lexer->rules) return CLEX_STATUS_OK;
  for (int i = 0; i < CLEX_MAX_RULES; ++i) {
    clexRule* rule = lexer->rules[i];
    if (!rule) continue;
    if (!add_expected_kind_unique(&lexer->last_error, rule->kind)) {
      return CLEX_STATUS_OUT_OF_MEMORY;
    }
  }
  return CLEX_STATUS_OK;
}

const clexError* clexGetLastError(const clexLexer* lexer) {
  if (!lexer) return NULL;
  return &lexer->last_error;
}

clexLexer* clexInit(void) {
  clexLexer* lexer = malloc(sizeof(clexLexer));
  if (!lexer) return NULL;
  lexer->rules = NULL;
  lexer->content = NULL;
  lexer->position = 0;
  lexer->line = 1;
  lexer->column = 1;
  clexErrorInit(&lexer->last_error);
  return lexer;
}

void clexLexerDestroy(clexLexer* lexer) {
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
  clexErrorClear(&lexer->last_error);
  free(lexer);
}

void clexReset(clexLexer* lexer, const char* content) {
  if (!lexer) return;
  lexer->content = content;
  lexer->position = 0;
  lexer->line = 1;
  lexer->column = 1;
  clexErrorClear(&lexer->last_error);
}

clexStatus clexRegisterKind(clexLexer* lexer, const char* re, int kind) {
  if (!lexer || !re) {
    return CLEX_STATUS_INVALID_ARGUMENT;
  }

  clexErrorClear(&lexer->last_error);

  if (!lexer->rules) {
    lexer->rules = calloc(CLEX_MAX_RULES, sizeof(clexRule*));
    if (!lexer->rules) {
      return lexer_set_error(
          lexer, CLEX_STATUS_OUT_OF_MEMORY,
          make_position(lexer->position, lexer->line, lexer->column), NULL);
    }
  }

  for (int i = 0; i < CLEX_MAX_RULES; i++) {
    if (!lexer->rules[i]) {
      clexRule* rule = malloc(sizeof(clexRule));
      if (!rule) {
        return lexer_set_error(
            lexer, CLEX_STATUS_OUT_OF_MEMORY,
            make_position(lexer->position, lexer->line, lexer->column), NULL);
      }
      rule->re = re;
      rule->nfa = clexNfaFromRe(re, NULL);
      if (!rule->nfa) {
        free(rule);
        return lexer_set_error(
            lexer, CLEX_STATUS_REGEX_ERROR,
            make_position(lexer->position, lexer->line, lexer->column), re);
      }
      rule->kind = kind;
      lexer->rules[i] = rule;
      return CLEX_STATUS_OK;
    }
  }

  return lexer_set_error(
      lexer, CLEX_STATUS_RULE_LIMIT_REACHED,
      make_position(lexer->position, lexer->line, lexer->column), re);
}

void clexDeleteKinds(clexLexer* lexer) {
  if (!lexer) return;
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

clexStatus clex(clexLexer* lexer, clexToken* out_token) {
  if (!lexer || !out_token) return CLEX_STATUS_INVALID_ARGUMENT;

  clexTokenClear(out_token);
  clexErrorClear(&lexer->last_error);

  out_token->kind = CLEX_TOKEN_EOF;
  out_token->span.start =
      make_position(lexer->position, lexer->line, lexer->column);
  out_token->span.end = out_token->span.start;

  if (!lexer->content) {
    return CLEX_STATUS_EOF;
  }

  const char* content = lexer->content;
  size_t length = strlen(content);

  while (lexer->position < length &&
         isspace((unsigned char)content[lexer->position])) {
    if (content[lexer->position] == '\n') {
      lexer->line++;
      lexer->column = 1;
    } else {
      lexer->column++;
    }
    lexer->position++;
  }

  if (lexer->position >= length) {
    out_token->span.start =
        make_position(lexer->position, lexer->line, lexer->column);
    out_token->span.end = out_token->span.start;
    return CLEX_STATUS_EOF;
  }

  if (!lexer->rules) {
    return lexer_set_error(
        lexer, CLEX_STATUS_NO_RULES,
        make_position(lexer->position, lexer->line, lexer->column), NULL);
  }

  size_t start = lexer->position;
  clexSourcePosition start_position =
      make_position(lexer->position, lexer->line, lexer->column);
  size_t end = start;
  while (content[end] != '\0' && !isspace((unsigned char)content[end])) end++;

  size_t partLength = end - start;
  if (partLength == 0) {
    lexer->position = end;
    out_token->span.start = make_position(end, lexer->line, lexer->column);
    out_token->span.end = out_token->span.start;
    return CLEX_STATUS_EOF;
  }

  char* part = calloc(partLength + 1, sizeof(char));
  if (!part) {
    return lexer_set_error(lexer, CLEX_STATUS_OUT_OF_MEMORY, start_position,
                           NULL);
  }
  memcpy(part, content + start, partLength);

  size_t activeLength = partLength;
  while (activeLength > 0) {
    for (int i = 0; i < CLEX_MAX_RULES; i++) {
      clexRule* rule = lexer->rules[i];
      if (rule && clexNfaTest(rule->nfa, part)) {
        out_token->lexeme = part;
        out_token->kind = rule->kind;
        out_token->span.start = start_position;
        out_token->span.end =
            advance_position(start_position, content + start, activeLength);
        lexer->position = start + activeLength;
        lexer->line = out_token->span.end.line;
        lexer->column = out_token->span.end.column;
        return CLEX_STATUS_OK;
      }
    }
    activeLength--;
    part[activeLength] = '\0';
  }

  free(part);
  char unmatched[2] = {content[start], '\0'};
  clexStatus status = lexer_set_error(lexer, CLEX_STATUS_LEXICAL_ERROR,
                                      start_position, unmatched);
  if (status == CLEX_STATUS_LEXICAL_ERROR) {
    if (lexer_fill_expected_kinds(lexer) == CLEX_STATUS_OUT_OF_MEMORY) {
      return lexer_set_error(lexer, CLEX_STATUS_OUT_OF_MEMORY, start_position,
                             NULL);
    }
  }
  out_token->kind = CLEX_TOKEN_ERROR;
  out_token->span.start = start_position;
  out_token->span.end = advance_position(start_position, content + start, 1);
  lexer->position = start + 1;
  lexer->line = out_token->span.end.line;
  lexer->column = out_token->span.end.column;
  return status;
}
