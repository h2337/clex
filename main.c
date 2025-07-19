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
  OCURLYBRACE,
  CCURLYBRACE,
  COMMA,
  COLON,
  EQUAL,
  OPARAN,
  CPARAN,
  OSQUAREBRACE,
  CSQUAREBRACE,
  DOT,
  AMPER,
  EXCLAMATION,
  TILDE,
  MINUS,
  PLUS,
  STAR,
  SLASH,
  PERCENT,
  RANGLE,
  LANGLE,
  CARET,
  PIPE,
  QUESTION,
  STRINGLITERAL,
  CONSTANT,
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
  registerKind("\\+=", ADD_ASSIGN);
  registerKind("-=", SUB_ASSIGN);
  registerKind("\\*=", MUL_ASSIGN);
  registerKind("/=", DIV_ASSIGN);
  registerKind("%=", MOD_ASSIGN);
  registerKind("&=", AND_ASSIGN);
  registerKind("^=", XOR_ASSIGN);
  registerKind("\\|=", OR_ASSIGN);
  registerKind(">>", RIGHT_OP);
  registerKind("<<", LEFT_OP);
  registerKind("\\+\\+", INC_OP);
  registerKind("--", DEC_OP);
  registerKind("->", PTR_OP);
  registerKind("&&", AND_OP);
  registerKind("\\|\\|", OR_OP);
  registerKind("<=", LE_OP);
  registerKind(">=", GE_OP);
  registerKind("==", EQ_OP);
  registerKind("!=", NE_OP);
  registerKind(";", SEMICOL);
  registerKind("{|<%", OCURLYBRACE);
  registerKind("}|%>", CCURLYBRACE);
  registerKind(",", COMMA);
  registerKind(":", COLON);
  registerKind("=", EQUAL);
  registerKind("\\(", OPARAN);
  registerKind("\\)", CPARAN);
  registerKind("\\[|<:", OSQUAREBRACE);
  registerKind("\\]|:>", CSQUAREBRACE);
  registerKind(".", DOT);
  registerKind("&", AMPER);
  registerKind("!", EXCLAMATION);
  registerKind("~", TILDE);
  registerKind("-", MINUS);
  registerKind("\\+", PLUS);
  registerKind("\\*", STAR);
  registerKind("/", SLASH);
  registerKind("%", PERCENT);
  registerKind("<", RANGLE);
  registerKind(">", LANGLE);
  registerKind("^", CARET);
  registerKind("\\|", PIPE);
  registerKind("\\?", QUESTION);
  registerKind("L?\"[ -~]*\"", STRINGLITERAL);
  registerKind("0[xX][a-fA-F0-9]+[uU]?[lL]?[lL]?", CONSTANT);
  registerKind("0[0-7]*[uU]?[lL]?[lL]?", CONSTANT);
  registerKind("[1-9][0-9]*", CONSTANT); // TODO: Remove when []? is fixed
  registerKind("[1-9][0-9]*[uU]?[lL]?[lL]?", CONSTANT);
  registerKind("L?'[ -~]*'", CONSTANT);
  registerKind("[0-9]+[Ee][+-]?[0-9]+[fFlL]", CONSTANT);
  registerKind("[0-9]*.[0-9]+[Ee][+-]?[fFlL]", CONSTANT);
  registerKind("[0-9]+.[0-9]*[Ee][+-]?[fFlL]", CONSTANT);
  registerKind("0[xX][a-fA-F0-9]+[Pp][+-]?[0-9]+[fFlL]?", CONSTANT);
  registerKind("0[xX][a-fA-F0-9]*.[a-fA-F0-9]+[Pp][+-]?[0-9]+[fFlL]?", CONSTANT);
  registerKind("0[xX][a-fA-F0-9]+.[a-fA-F0-9]+[Pp][+-]?[0-9]+[fFlL]?", CONSTANT);
  registerKind("[a-zA-Z_]([a-zA-Z_]|[0-9])*", IDENTIFIER);
  // TODO: Add comment // and /* */
  // TODO: Add #

  /*
  FILE *f = fopen("./fa.c", "r");
  fseek(f, 0L, SEEK_END);
  size_t fsize = ftell(f);
  rewind(f);

  char *fcontent = malloc(fsize + 1);
  fread(fcontent, fsize, 1, f);
  fcontent[fsize] = '\0';
  fclose(f);
  */

  initClex("int main(int argc, char *argv[]) {\nreturn 23;\n}");

  token = clex();
  assert(token->kind == INT);
  assert(strcmp(token->lexeme, "int") == 0);

  token = clex();
  assert(token->kind == IDENTIFIER);
  assert(strcmp(token->lexeme, "main") == 0);

  token = clex();
  assert(token->kind == OPARAN);
  assert(strcmp(token->lexeme, "(") == 0);

  token = clex();
  assert(token->kind == INT);
  assert(strcmp(token->lexeme, "int") == 0);

  token = clex();
  assert(token->kind == IDENTIFIER);
  assert(strcmp(token->lexeme, "argc") == 0);

  token = clex();
  assert(token->kind == COMMA);
  assert(strcmp(token->lexeme, ",") == 0);

  token = clex();
  assert(token->kind == CHAR);
  assert(strcmp(token->lexeme, "char") == 0);

  token = clex();
  assert(token->kind == STAR);
  assert(strcmp(token->lexeme, "*") == 0);

  token = clex();
  assert(token->kind == IDENTIFIER);
  assert(strcmp(token->lexeme, "argv") == 0);

  token = clex();
  assert(token->kind == OSQUAREBRACE);
  assert(strcmp(token->lexeme, "[") == 0);

  token = clex();
  assert(token->kind == CSQUAREBRACE);
  assert(strcmp(token->lexeme, "]") == 0);

  token = clex();
  assert(token->kind == CPARAN);
  assert(strcmp(token->lexeme, ")") == 0);

  token = clex();
  assert(token->kind == OCURLYBRACE);
  assert(strcmp(token->lexeme, "{") == 0);

  token = clex();
  assert(token->kind == RETURN);
  assert(strcmp(token->lexeme, "return") == 0);

  token = clex();
  assert(token->kind == CONSTANT);
  assert(strcmp(token->lexeme, "23") == 0);

  token = clex();
  assert(token->kind == SEMICOL);
  assert(strcmp(token->lexeme, ";") == 0);

  token = clex();
  assert(token->kind == CCURLYBRACE);
  assert(strcmp(token->lexeme, "}") == 0);
}
#endif

#ifdef TEST_REGEX

#include "fa.h"
#include <assert.h>

int main(int argc, char *argv) {
  Node *nfa = reToNFA("a");
  assert(test(nfa, "a") == true);
  assert(test(nfa, "b") == false);

  nfa = reToNFA("ab");
  assert(test(nfa, "ab") == true);
  assert(test(nfa, "a") == false);
  assert(test(nfa, "b") == false);

  nfa = reToNFA("ab|c");
  assert(test(nfa, "ab") == true);
  assert(test(nfa, "c") == true);
  assert(test(nfa, "abc") == false);

  nfa = reToNFA("ab*c");
  assert(test(nfa, "c") == true);
  assert(test(nfa, "abc") == true);
  assert(test(nfa, "ababc") == true);
  assert(test(nfa, "ab") == false);
  assert(test(nfa, "abd") == false);
  assert(test(nfa, "acc") == false);

  nfa = reToNFA("ab+c");
  assert(test(nfa, "c") == false);
  assert(test(nfa, "abc") == true);
  assert(test(nfa, "ababc") == true);
  assert(test(nfa, "ab") == false);
  assert(test(nfa, "abd") == false);
  assert(test(nfa, "acc") == false);

  nfa = reToNFA("ab?c");
  assert(test(nfa, "c") == true);
  assert(test(nfa, "abc") == true);
  assert(test(nfa, "ababc") == false);
  assert(test(nfa, "ab") == false);
  assert(test(nfa, "abd") == false);
  assert(test(nfa, "acc") == false);

  nfa = reToNFA("[ab]c");
  assert(test(nfa, "c") == false);
  assert(test(nfa, "ac") == true);
  assert(test(nfa, "bc") == true);
  assert(test(nfa, "abc") == false);
  assert(test(nfa, "bd") == false);
  assert(test(nfa, "acc") == false);

  nfa = reToNFA("[A-Za-z]c");
  assert(test(nfa, "c") == false);
  assert(test(nfa, "ac") == true);
  assert(test(nfa, "bc") == true);
  assert(test(nfa, "Ac") == true);
  assert(test(nfa, "Zd") == false);
  assert(test(nfa, "Zc") == true);

  nfa = reToNFA("[A-Za-z]*c");
  assert(test(nfa, "AZazc") == true);
  assert(test(nfa, "AZaz") == false);

  nfa = reToNFA("[A-Za-z]?c");
  assert(test(nfa, "Ac") == true);
  assert(test(nfa, "c") == true);
  assert(test(nfa, "A") == false);

  nfa = reToNFA("a(bc|de)f");
  assert(test(nfa, "abcf") == true);
  assert(test(nfa, "adef") == true);
  assert(test(nfa, "af") == false);
  assert(test(nfa, "abf") == false);
  assert(test(nfa, "abcdef") == false);
  assert(test(nfa, "abccf") == false);
  assert(test(nfa, "bcf") == false);
  assert(test(nfa, "abc") == false);

  nfa = reToNFA("(bc|de)f");
  assert(test(nfa, "bcf") == true);
  assert(test(nfa, "def") == true);

  nfa = reToNFA("a(bc)*f");
  assert(test(nfa, "af") == true);
  assert(test(nfa, "abcf") == true);
  assert(test(nfa, "abcbcf") == true);
  assert(test(nfa, "abcbf") == false);

  nfa = reToNFA("(bc)*f");
  assert(test(nfa, "f") == true);
  assert(test(nfa, "bcf") == true);
  assert(test(nfa, "bcbcf") == true);
  assert(test(nfa, "bcbf") == false);
  assert(test(nfa, "bc") == false);

  nfa = reToNFA("a(bc|de)*f");
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

  nfa = reToNFA("a(bc|de)+f");
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

  nfa = reToNFA("a(bc|de)?f");
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

  nfa = reToNFA("([a-zA-Z_])*");
  assert(test(nfa, "valid") == true);
  assert(test(nfa, "Valid") == true);
  assert(test(nfa, "_var1") == false);
  assert(test(nfa, "vv1") == false);
  assert(test(nfa, "v1") == false);

  nfa = reToNFA("([a-zA-Z_]|[0-9])*");
  assert(test(nfa, "valid") == true);
  assert(test(nfa, "Valid") == true);
  assert(test(nfa, "_var1") == true);
  assert(test(nfa, "vv1") == true);
  assert(test(nfa, "v1") == true);

  nfa = reToNFA("[a-zA-Z_]([a-zA-Z_]|[0-9])*");
  assert(test(nfa, "valid") == true);
  assert(test(nfa, "Valid") == true);
  assert(test(nfa, "_var1") == true);
  assert(test(nfa, "vv1") == true);
  assert(test(nfa, "v1") == true);

  nfa = reToNFA("\\|");
  assert(test(nfa, "|") == true);

  nfa = reToNFA("\\[");
  assert(test(nfa, "[") == true);

  nfa = reToNFA("\\]");
  assert(test(nfa, "]") == true);

  nfa = reToNFA("\\+");
  assert(test(nfa, "+") == true);

  nfa = reToNFA("\\*");
  assert(test(nfa, "*") == true);

  nfa = reToNFA("\\?");
  assert(test(nfa, "?") == true);

  // TODO: Fix
  //nfa = reToNFA("[1-9][0-9]*[uU]?[lL]?[lL]?");
  //assert(test(nfa, "23") == true);
}
#endif
