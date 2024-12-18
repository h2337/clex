#ifdef TEST_CLEX

#include "clex.h"
#include <assert.h>
#include <string.h>

typedef enum TokenKind {
  EOF,
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
  clexRegisterKind("auto", AUTO);
  clexRegisterKind("_Bool", BOOL);
  clexRegisterKind("break", BREAK);
  clexRegisterKind("[a-zA-Z_]([a-zA-Z_]|[0-9])*", IDENTIFIER);
  clexRegisterKind(";", SEMICOL);

  clexInit("auto ident1; break;");

  Token token = clex();
  assert(token.kind == AUTO);
  assert(strcmp(token.lexeme, "auto") == 0);

  token = clex();
  assert(token.kind == IDENTIFIER);
  assert(strcmp(token.lexeme, "ident1") == 0);

  token = clex();
  assert(token.kind == SEMICOL);
  assert(strcmp(token.lexeme, ";") == 0);

  token = clex();
  assert(token.kind == BREAK);
  assert(strcmp(token.lexeme, "break") == 0);

  token = clex();
  assert(token.kind == SEMICOL);
  assert(strcmp(token.lexeme, ";") == 0);

  clexDeleteKinds();

  clexRegisterKind("auto", AUTO);
  clexRegisterKind("_Bool", BOOL);
  clexRegisterKind("break", BREAK);
  clexRegisterKind("case", CASE);
  clexRegisterKind("char", CHAR);
  clexRegisterKind("_Complex", COMPLEX);
  clexRegisterKind("const", CONST);
  clexRegisterKind("continue", CONTINUE);
  clexRegisterKind("default", DEFAULT);
  clexRegisterKind("do", DO);
  clexRegisterKind("double", DOUBLE);
  clexRegisterKind("else", ELSE);
  clexRegisterKind("enum", ENUM);
  clexRegisterKind("extern", EXTERN);
  clexRegisterKind("float", FLOAT);
  clexRegisterKind("for", FOR);
  clexRegisterKind("goto", GOTO);
  clexRegisterKind("if", IF);
  clexRegisterKind("_Imaginary", IMAGINARY);
  clexRegisterKind("inline", INLINE);
  clexRegisterKind("int", INT);
  clexRegisterKind("long", LONG);
  clexRegisterKind("register", REGISTER);
  clexRegisterKind("restrict", RESTRICT);
  clexRegisterKind("return", RETURN);
  clexRegisterKind("short", SHORT);
  clexRegisterKind("signed", SIGNED);
  clexRegisterKind("sizeof", SIZEOF);
  clexRegisterKind("static", STATIC);
  clexRegisterKind("struct", STRUCT);
  clexRegisterKind("switch", SWITCH);
  clexRegisterKind("typedef", TYPEDEF);
  clexRegisterKind("union", UNION);
  clexRegisterKind("unsigned", UNSIGNED);
  clexRegisterKind("void", VOID);
  clexRegisterKind("volatile", VOLATILE);
  clexRegisterKind("while", WHILE);
  clexRegisterKind("...", ELLIPSIS);
  clexRegisterKind(">>=", RIGHT_ASSIGN);
  clexRegisterKind("<<=", LEFT_ASSIGN);
  clexRegisterKind("\\+=", ADD_ASSIGN);
  clexRegisterKind("-=", SUB_ASSIGN);
  clexRegisterKind("\\*=", MUL_ASSIGN);
  clexRegisterKind("/=", DIV_ASSIGN);
  clexRegisterKind("%=", MOD_ASSIGN);
  clexRegisterKind("&=", AND_ASSIGN);
  clexRegisterKind("^=", XOR_ASSIGN);
  clexRegisterKind("\\|=", OR_ASSIGN);
  clexRegisterKind(">>", RIGHT_OP);
  clexRegisterKind("<<", LEFT_OP);
  clexRegisterKind("\\+\\+", INC_OP);
  clexRegisterKind("--", DEC_OP);
  clexRegisterKind("->", PTR_OP);
  clexRegisterKind("&&", AND_OP);
  clexRegisterKind("\\|\\|", OR_OP);
  clexRegisterKind("<=", LE_OP);
  clexRegisterKind(">=", GE_OP);
  clexRegisterKind("==", EQ_OP);
  clexRegisterKind("!=", NE_OP);
  clexRegisterKind(";", SEMICOL);
  clexRegisterKind("{|<%", OCURLYBRACE);
  clexRegisterKind("}|%>", CCURLYBRACE);
  clexRegisterKind(",", COMMA);
  clexRegisterKind(":", COLON);
  clexRegisterKind("=", EQUAL);
  clexRegisterKind("\\(", OPARAN);
  clexRegisterKind("\\)", CPARAN);
  clexRegisterKind("\\[|<:", OSQUAREBRACE);
  clexRegisterKind("\\]|:>", CSQUAREBRACE);
  clexRegisterKind(".", DOT);
  clexRegisterKind("&", AMPER);
  clexRegisterKind("!", EXCLAMATION);
  clexRegisterKind("~", TILDE);
  clexRegisterKind("-", MINUS);
  clexRegisterKind("\\+", PLUS);
  clexRegisterKind("\\*", STAR);
  clexRegisterKind("/", SLASH);
  clexRegisterKind("%", PERCENT);
  clexRegisterKind("<", RANGLE);
  clexRegisterKind(">", LANGLE);
  clexRegisterKind("^", CARET);
  clexRegisterKind("\\|", PIPE);
  clexRegisterKind("\\?", QUESTION);
  clexRegisterKind("L?\"[ -~]*\"", STRINGLITERAL);
  clexRegisterKind("0[xX][a-fA-F0-9]+([uU])?([lL])?([lL])?", CONSTANT);
  clexRegisterKind("0[0-7]*([uU])?([lL])?([lL])?", CONSTANT);
  clexRegisterKind("[1-9][0-9]*([uU])?([lL])?([lL])?", CONSTANT);
  clexRegisterKind("L?'[ -~]*'", CONSTANT);
  clexRegisterKind("[0-9]+[Ee][+-]?[0-9]+[fFlL]", CONSTANT);
  clexRegisterKind("[0-9]*.[0-9]+[Ee][+-]?[fFlL]", CONSTANT);
  clexRegisterKind("[0-9]+.[0-9]*[Ee][+-]?[fFlL]", CONSTANT);
  clexRegisterKind("0[xX][a-fA-F0-9]+[Pp][+-]?[0-9]+([fFlL])?", CONSTANT);
  clexRegisterKind("0[xX][a-fA-F0-9]*.[a-fA-F0-9]+[Pp][+-]?[0-9]+([fFlL])?", CONSTANT);
  clexRegisterKind("0[xX][a-fA-F0-9]+.[a-fA-F0-9]+[Pp][+-]?[0-9]+([fFlL])?", CONSTANT);
  clexRegisterKind("[a-zA-Z_]([a-zA-Z_]|[0-9])*", IDENTIFIER);
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

  clexInit("int main(int argc, char *argv[]) {\nreturn 23;\n}");

  token = clex();
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
#endif

#ifdef TEST_REGEX

#include "fa.h"
#include <assert.h>

int main(int argc, char *argv) {
  Node *nfa = NFAFromRe("a");
  assert(NFATest(nfa, "a") == true);
  assert(NFATest(nfa, "b") == false);

  nfa = NFAFromRe("ab");
  assert(NFATest(nfa, "ab") == true);
  assert(NFATest(nfa, "a") == false);
  assert(NFATest(nfa, "b") == false);

  nfa = NFAFromRe("ab|c");
  assert(NFATest(nfa, "ab") == true);
  assert(NFATest(nfa, "c") == true);
  assert(NFATest(nfa, "abc") == false);

  nfa = NFAFromRe("ab*c");
  assert(NFATest(nfa, "c") == true);
  assert(NFATest(nfa, "abc") == true);
  assert(NFATest(nfa, "ababc") == true);
  assert(NFATest(nfa, "ab") == false);
  assert(NFATest(nfa, "abd") == false);
  assert(NFATest(nfa, "acc") == false);

  nfa = NFAFromRe("ab+c");
  assert(NFATest(nfa, "c") == false);
  assert(NFATest(nfa, "abc") == true);
  assert(NFATest(nfa, "ababc") == true);
  assert(NFATest(nfa, "ab") == false);
  assert(NFATest(nfa, "abd") == false);
  assert(NFATest(nfa, "acc") == false);

  nfa = NFAFromRe("ab?c");
  assert(NFATest(nfa, "c") == true);
  assert(NFATest(nfa, "abc") == true);
  assert(NFATest(nfa, "ababc") == false);
  assert(NFATest(nfa, "ab") == false);
  assert(NFATest(nfa, "abd") == false);
  assert(NFATest(nfa, "acc") == false);

  nfa = NFAFromRe("[ab]c");
  assert(NFATest(nfa, "c") == false);
  assert(NFATest(nfa, "ac") == true);
  assert(NFATest(nfa, "bc") == true);
  assert(NFATest(nfa, "abc") == false);
  assert(NFATest(nfa, "bd") == false);
  assert(NFATest(nfa, "acc") == false);

  nfa = NFAFromRe("[A-Za-z]c");
  assert(NFATest(nfa, "c") == false);
  assert(NFATest(nfa, "ac") == true);
  assert(NFATest(nfa, "bc") == true);
  assert(NFATest(nfa, "Ac") == true);
  assert(NFATest(nfa, "Zd") == false);
  assert(NFATest(nfa, "Zc") == true);

  nfa = NFAFromRe("[A-Za-z]*c");
  assert(NFATest(nfa, "AZazc") == true);
  assert(NFATest(nfa, "AZaz") == false);

  nfa = NFAFromRe("[A-Za-z]?c");
  assert(NFATest(nfa, "Ac") == true);
  assert(NFATest(nfa, "c") == true);
  assert(NFATest(nfa, "A") == false);

  nfa = NFAFromRe("a(bc|de)f");
  assert(NFATest(nfa, "abcf") == true);
  assert(NFATest(nfa, "adef") == true);
  assert(NFATest(nfa, "af") == false);
  assert(NFATest(nfa, "abf") == false);
  assert(NFATest(nfa, "abcdef") == false);
  assert(NFATest(nfa, "abccf") == false);
  assert(NFATest(nfa, "bcf") == false);
  assert(NFATest(nfa, "abc") == false);

  nfa = NFAFromRe("(bc|de)f");
  assert(NFATest(nfa, "bcf") == true);
  assert(NFATest(nfa, "def") == true);

  nfa = NFAFromRe("a(bc)*f");
  assert(NFATest(nfa, "af") == true);
  assert(NFATest(nfa, "abcf") == true);
  assert(NFATest(nfa, "abcbcf") == true);
  assert(NFATest(nfa, "abcbf") == false);

  nfa = NFAFromRe("(bc)*f");
  assert(NFATest(nfa, "f") == true);
  assert(NFATest(nfa, "bcf") == true);
  assert(NFATest(nfa, "bcbcf") == true);
  assert(NFATest(nfa, "bcbf") == false);
  assert(NFATest(nfa, "bc") == false);

  nfa = NFAFromRe("a(bc|de)*f");
  assert(NFATest(nfa, "af") == true);
  assert(NFATest(nfa, "abcf") == true);
  assert(NFATest(nfa, "adef") == true);
  assert(NFATest(nfa, "abcbcf") == true);
  assert(NFATest(nfa, "adedef") == true);
  assert(NFATest(nfa, "abcdef") == true);
  assert(NFATest(nfa, "abf") == false);
  assert(NFATest(nfa, "abccf") == false);
  assert(NFATest(nfa, "bcf") == false);
  assert(NFATest(nfa, "abc") == false);

  nfa = NFAFromRe("a(bc|de)+f");
  assert(NFATest(nfa, "af") == false);
  assert(NFATest(nfa, "abcf") == true);
  assert(NFATest(nfa, "adef") == true);
  assert(NFATest(nfa, "abcbcf") == true);
  assert(NFATest(nfa, "adedef") == true);
  assert(NFATest(nfa, "abcdef") == true);
  assert(NFATest(nfa, "abf") == false);
  assert(NFATest(nfa, "abccf") == false);
  assert(NFATest(nfa, "bcf") == false);
  assert(NFATest(nfa, "abc") == false);

  nfa = NFAFromRe("a(bc|de)?f");
  assert(NFATest(nfa, "af") == true);
  assert(NFATest(nfa, "abcf") == true);
  assert(NFATest(nfa, "adef") == true);
  assert(NFATest(nfa, "abcbcf") == false);
  assert(NFATest(nfa, "adedef") == false);
  assert(NFATest(nfa, "abcdef") == false);
  assert(NFATest(nfa, "abf") == false);
  assert(NFATest(nfa, "abccf") == false);
  assert(NFATest(nfa, "bcf") == false);
  assert(NFATest(nfa, "abc") == false);

  nfa = NFAFromRe("([a-zA-Z_])*");
  assert(NFATest(nfa, "valid") == true);
  assert(NFATest(nfa, "Valid") == true);
  assert(NFATest(nfa, "_var1") == false);
  assert(NFATest(nfa, "vv1") == false);
  assert(NFATest(nfa, "v1") == false);

  nfa = NFAFromRe("([a-zA-Z_]|[0-9])*");
  assert(NFATest(nfa, "valid") == true);
  assert(NFATest(nfa, "Valid") == true);
  assert(NFATest(nfa, "_var1") == true);
  assert(NFATest(nfa, "vv1") == true);
  assert(NFATest(nfa, "v1") == true);

  nfa = NFAFromRe("[a-zA-Z_]([a-zA-Z_]|[0-9])*");
  assert(NFATest(nfa, "valid") == true);
  assert(NFATest(nfa, "Valid") == true);
  assert(NFATest(nfa, "_var1") == true);
  assert(NFATest(nfa, "vv1") == true);
  assert(NFATest(nfa, "v1") == true);

  nfa = NFAFromRe("\\|");
  assert(NFATest(nfa, "|") == true);

  nfa = NFAFromRe("\\[");
  assert(NFATest(nfa, "[") == true);

  nfa = NFAFromRe("\\]");
  assert(NFATest(nfa, "]") == true);

  nfa = NFAFromRe("\\+");
  assert(NFATest(nfa, "+") == true);

  nfa = NFAFromRe("\\*");
  assert(NFATest(nfa, "*") == true);

  nfa = NFAFromRe("\\?");
  assert(NFATest(nfa, "?") == true);

  nfa = NFAFromRe("(u)(a)");
  assert(NFATest(nfa, "ua") == true);

  nfa = NFAFromRe("[1-9][0-9]*([uU])?([lL])?([lL])?");
  assert(NFATest(nfa, "23") == true);
  assert(NFATest(nfa, "23u") == true);
  assert(NFATest(nfa, "23UlL") == true);
}
#endif
