<img align="right" src="https://raw.githubusercontent.com/h2337/file-hosting/fdb7de03412e0b4e97488828e9c61117f4120457/clex.png">

## TOC

* [Overview](#overview)
* [Example](#example)

## Overview

clex is a simple lexer generator for C.

With clex you can associate a regex pattern to each token type with `registerKind(regex, type)` call, pass the source using `initClex(source)` call, and then lex the next token with `clex()` call.

## Example

```c
#include "clex.h"
#include <assert.h>
#include <string.h>

typedef enum TokenKind {
  INT,
  OPARAN,
  CPARAN,
  OSQUAREBRACE,
  CSQUAREBRACE,
  OCURLYBRACE,
  CCURLYBRACE,
  COMMA,
  CHAR,
  STAR,
  RETURN,
  SEMICOL,
  CONSTANT,
  IDENTIFIER,
} TokenKind;

int main(int argc, char *argv[]) {
  registerKind("int", INT);
  registerKind("\\(", OPARAN);
  registerKind("\\)", CPARAN);
  registerKind("\\[|<:", OSQUAREBRACE);
  registerKind("\\]|:>", CSQUAREBRACE);
  registerKind("{|<%", OCURLYBRACE);
  registerKind("}|%>", CCURLYBRACE);
  registerKind(",", COMMA);
  registerKind("char", CHAR);
  registerKind("\\*", STAR);
  registerKind("return", RETURN);
  registerKind("[1-9][0-9]*([uU])?([lL])?([lL])?", CONSTANT);
  registerKind(";", SEMICOL);
  registerKind("[a-zA-Z_]([a-zA-Z_]|[0-9])*", IDENTIFIER);

  initClex("int main(int argc, char *argv[]) {\nreturn 23;\n}");

  Token token = clex();
  assert(token.kind == INT);
  assert(strcmp(token.lexeme, "int") == 0);

  token = clex();
  assert(token.kind == IDENTIFIER);
  assert(strcmp(token.lexeme, "main") == 0);

  token = clex();
  assert(token.kind == OPARAN);
  assert(strcmp(token.lexeme, "(") == 0);

  token = clex();
  assert(token.kind == INT);
  assert(strcmp(token.lexeme, "int") == 0);

  token = clex();
  assert(token.kind == IDENTIFIER);
  assert(strcmp(token.lexeme, "argc") == 0);

  token = clex();
  assert(token.kind == COMMA);
  assert(strcmp(token.lexeme, ",") == 0);

  token = clex();
  assert(token.kind == CHAR);
  assert(strcmp(token.lexeme, "char") == 0);

  token = clex();
  assert(token.kind == STAR);
  assert(strcmp(token.lexeme, "*") == 0);

  token = clex();
  assert(token.kind == IDENTIFIER);
  assert(strcmp(token.lexeme, "argv") == 0);

  token = clex();
  assert(token.kind == OSQUAREBRACE);
  assert(strcmp(token.lexeme, "[") == 0);

  token = clex();
  assert(token.kind == CSQUAREBRACE);
  assert(strcmp(token.lexeme, "]") == 0);

  token = clex();
  assert(token.kind == CPARAN);
  assert(strcmp(token.lexeme, ")") == 0);

  token = clex();
  assert(token.kind == OCURLYBRACE);
  assert(strcmp(token.lexeme, "{") == 0);

  token = clex();
  assert(token.kind == RETURN);
  assert(strcmp(token.lexeme, "return") == 0);

  token = clex();
  assert(token.kind == CONSTANT);
  assert(strcmp(token.lexeme, "23") == 0);

  token = clex();
  assert(token.kind == SEMICOL);
  assert(strcmp(token.lexeme, ";") == 0);

  token = clex();
  assert(token.kind == CCURLYBRACE);
  assert(strcmp(token.lexeme, "}") == 0);
}
```
