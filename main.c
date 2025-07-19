#ifdef TEST_CLEX

#include "clex.h"
#include <assert.h>
#include <string.h>

typedef enum TokenKind {
  AUTO,
  BOOL,
  BREAK,
  SEMICOL,
  IDENTIFIER,
} TokenKind;

int main(int argc, char *argv[]) {
  registerKind("auto", AUTO);
  registerKind("_Bool", BOOL);
  registerKind("break", BREAK);
  registerKind("[a-zA-Z_]([a-zA-Z_]|[0-9])*", IDENTIFIER);
  registerKind(";", SEMICOL);

  initClex("auto ident1; break;");
  Token *token = clex();

  assert(token->kind == AUTO);
  assert(strcmp(token->lexeme, "auto") == 0);

  token = clex();
  assert(token->kind == IDENTIFIER);
  assert(strcmp(token->lexeme, "ident1") == 0);

  token = clex();
  assert(token->kind == SEMICOL);
  assert(strcmp(token->lexeme, ";") == 0);

  token = clex();
  assert(token->kind == BREAK);
  assert(strcmp(token->lexeme, "break") == 0);

  token = clex();
  assert(token->kind == SEMICOL);
  assert(strcmp(token->lexeme, ";") == 0);

  deleteKinds();
}
#endif

#ifdef TEST_REGEX

#include "fa.h"
#include <assert.h>

int main(int argc, char *argv) {
  // TODO: Move initLexer into reToNFA
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

  initLexer("[A-Za-z]*c");
  nfa = reToNFA();
  assert(test(nfa, "AZazc") == true);
  assert(test(nfa, "AZaz") == false);

  initLexer("[A-Za-z]?c");
  nfa = reToNFA();
  assert(test(nfa, "Ac") == true);
  assert(test(nfa, "c") == true);
  assert(test(nfa, "A") == false);

  initLexer("a(bc|de)f");
  nfa = reToNFA();
  assert(test(nfa, "abcf") == true);
  assert(test(nfa, "adef") == true);
  assert(test(nfa, "af") == false);
  assert(test(nfa, "abf") == false);
  assert(test(nfa, "abcdef") == false);
  assert(test(nfa, "abccf") == false);
  assert(test(nfa, "bcf") == false);
  assert(test(nfa, "abc") == false);

  initLexer("(bc|de)f");
  nfa = reToNFA();
  assert(test(nfa, "bcf") == true);
  assert(test(nfa, "def") == true);

  initLexer("a(bc)*f");
  nfa = reToNFA();
  assert(test(nfa, "af") == true);
  assert(test(nfa, "abcf") == true);
  assert(test(nfa, "abcbcf") == true);
  assert(test(nfa, "abcbf") == false);

  initLexer("(bc)*f");
  nfa = reToNFA();
  assert(test(nfa, "f") == true);
  assert(test(nfa, "bcf") == true);
  assert(test(nfa, "bcbcf") == true);
  assert(test(nfa, "bcbf") == false);
  assert(test(nfa, "bc") == false);

  initLexer("a(bc|de)*f");
  nfa = reToNFA();
  assert(test(nfa, "af") == true);
  assert(test(nfa, "abcf") == true);
  assert(test(nfa, "adef") == true);
  assert(test(nfa, "abcbcf") == true);
  assert(test(nfa, "adedef") == true);
  assert(test(nfa, "abcdef") == true);
  assert(test(nfa, "abf") == false);
  assert(test(nfa, "abccf") == false);
  assert(test(nfa, "bcf") == false);
  assert(test(nfa, "abc") == false);

  initLexer("a(bc|de)+f");
  nfa = reToNFA();
  assert(test(nfa, "af") == false);
  assert(test(nfa, "abcf") == true);
  assert(test(nfa, "adef") == true);
  assert(test(nfa, "abcbcf") == true);
  assert(test(nfa, "adedef") == true);
  assert(test(nfa, "abcdef") == true);
  assert(test(nfa, "abf") == false);
  assert(test(nfa, "abccf") == false);
  assert(test(nfa, "bcf") == false);
  assert(test(nfa, "abc") == false);

  initLexer("a(bc|de)?f");
  nfa = reToNFA();
  assert(test(nfa, "af") == true);
  assert(test(nfa, "abcf") == true);
  assert(test(nfa, "adef") == true);
  assert(test(nfa, "abcbcf") == false);
  assert(test(nfa, "adedef") == false);
  assert(test(nfa, "abcdef") == false);
  assert(test(nfa, "abf") == false);
  assert(test(nfa, "abccf") == false);
  assert(test(nfa, "bcf") == false);
  assert(test(nfa, "abc") == false);

  initLexer("([a-zA-Z_])*");
  nfa = reToNFA();
  assert(test(nfa, "valid") == true);
  assert(test(nfa, "Valid") == true);
  assert(test(nfa, "_var1") == false);
  assert(test(nfa, "vv1") == false);
  assert(test(nfa, "v1") == false);

  initLexer("([a-zA-Z_]|[0-9])*");
  nfa = reToNFA();
  assert(test(nfa, "valid") == true);
  assert(test(nfa, "Valid") == true);
  assert(test(nfa, "_var1") == true);
  assert(test(nfa, "vv1") == true);
  assert(test(nfa, "v1") == true);

  initLexer("[a-zA-Z_]([a-zA-Z_]|[0-9])*");
  nfa = reToNFA();
  assert(test(nfa, "valid") == true);
  assert(test(nfa, "Valid") == true);
  assert(test(nfa, "_var1") == true);
  assert(test(nfa, "vv1") == true);
  assert(test(nfa, "v1") == true);
}
#endif
