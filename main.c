#ifdef TEST_CLEX

#include "clex.h"
#include <assert.h>
#include <string.h>

typedef enum TokenKind {
  AUTO,
  BOOL,
  BREAK,
  CASE,
  CHAR,
  COMPLEX,
  CONST,
  CONTINUE,
  DEFAULT,
  DO,
  DOUBLE,
  ELSE,
  ENUM,
  EXTERN,
  FLOAT,
  FOR,
  GOTO,
  IF,
  IMAGINARY,
  INLINE,
  INT,
  LONG,
  REGISTER,
  RESTRICT,
  RETURN,
  SHORT,
  SIGNED,
  SIZEOF,
  STATIC,
  STRUCT,
  SWITCH,
  TYPEDEF,
  UNION,
  UNSIGNED,
  VOID,
  VOLATILE,
  WHILE,
  ELLIPSIS,
  RIGHT_ASSIGN,
  LEFT_ASSIGN,
  ADD_ASSIGN,
  SUB_ASSIGN,
  MUL_ASSIGN,
  DIV_ASSIGN,
  MOD_ASSIGN,
  AND_ASSIGN,
  XOR_ASSIGN,
  OR_ASSIGN,
  RIGHT_OP,
  LEFT_OP,
  INC_OP,
  DEC_OP,
  PTR_OP,
  AND_OP,
  OR_OP,
  LE_OP,
  GE_OP,
  EQ_OP,
  NE_OP,
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

  registerKind("auto", AUTO);
  registerKind("_Bool", BOOL);
  registerKind("break", BREAK);
  registerKind("case", CASE);
  registerKind("char", CHAR);
  registerKind("_Complex", COMPLEX);
  registerKind("const", CONST);
  registerKind("continue", CONTINUE);
  registerKind("default", DEFAULT);
  registerKind("do", DO);
  registerKind("double", DOUBLE);
  registerKind("else", ELSE);
  registerKind("enum", ENUM);
  registerKind("extern", EXTERN);
  registerKind("float", FLOAT);
  registerKind("for", FOR);
  registerKind("goto", GOTO);
  registerKind("if", IF);
  registerKind("_Imaginary", IMAGINARY);
  registerKind("inline", INLINE);
  registerKind("int", INT);
  registerKind("long", LONG);
  registerKind("register", REGISTER);
  registerKind("restrict", RESTRICT);
  registerKind("return", RETURN);
  registerKind("short", SHORT);
  registerKind("signed", SIGNED);
  registerKind("sizeof", SIZEOF);
  registerKind("static", STATIC);
  registerKind("struct", STRUCT);
  registerKind("switch", SWITCH);
  registerKind("typedef", TYPEDEF);
  registerKind("union", UNION);
  registerKind("unsigned", UNSIGNED);
  registerKind("void", VOID);
  registerKind("volatile", VOLATILE);
  registerKind("while", WHILE);
  registerKind("...", ELLIPSIS);
  registerKind(">>=", RIGHT_ASSIGN);
  registerKind("<<=", LEFT_ASSIGN);
  registerKind("+=", ADD_ASSIGN);
  registerKind("-=", SUB_ASSIGN);
  registerKind("*=", MUL_ASSIGN);
  registerKind("/=", DIV_ASSIGN);
  registerKind("%=", MOD_ASSIGN);
  registerKind("&=", AND_ASSIGN);
  registerKind("^=", XOR_ASSIGN);
  registerKind("|=", OR_ASSIGN);
  registerKind(">>", RIGHT_OP);
  registerKind("<<", LEFT_OP);
  registerKind("++", INC_OP);
  registerKind("--", DEC_OP);
  registerKind("->", PTR_OP);
  registerKind("&&", AND_OP);
  registerKind("||", OR_OP);
  registerKind("<=", LE_OP);
  registerKind(">=", GE_OP);
  registerKind("==", EQ_OP);
  registerKind("!=", NE_OP);
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
