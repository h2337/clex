#ifdef TEST_CLEX

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "clex.h"

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

int main(void) {
  clexLexer* lexer = clexInit();

  assert(clexRegisterKind(NULL, "auto", AUTO) == false);
  assert(clexRegisterKind(lexer, NULL, AUTO) == false);
  assert(clexRegisterKind(lexer, "[", AUTO) == false);
  assert(clexRegisterKind(lexer, "\\", AUTO) == false);
  assert(clexRegisterKind(lexer, "(", AUTO) == false);
  assert(clexRegisterKind(lexer, "a(", AUTO) == false);
  assert(clexRegisterKind(lexer, "a|", AUTO) == false);
  assert(clexRegisterKind(lexer, "|", AUTO) == false);
  assert(clexRegisterKind(lexer, "a)", AUTO) == false);

  clexRegisterKind(lexer, "auto", AUTO);
  clexRegisterKind(lexer, "_Bool", BOOL);
  clexRegisterKind(lexer, "break", BREAK);
  clexRegisterKind(lexer, "[a-zA-Z_]([a-zA-Z_]|[0-9])*", IDENTIFIER);
  clexRegisterKind(lexer, ";", SEMICOL);

  clexReset(lexer, NULL);
  clexToken token = clex(lexer);
  assert(token.kind == CLEX_TOKEN_EOF);
  assert(token.lexeme == NULL);

  clexReset(lexer, "auto ident1; break;");

  token = clex(lexer);
  assert(token.kind == AUTO);
  assert(strcmp(token.lexeme, "auto") == 0);

  token = clex(lexer);
  assert(token.kind == IDENTIFIER);
  assert(strcmp(token.lexeme, "ident1") == 0);

  token = clex(lexer);
  assert(token.kind == SEMICOL);
  assert(strcmp(token.lexeme, ";") == 0);

  token = clex(lexer);
  assert(token.kind == BREAK);
  assert(strcmp(token.lexeme, "break") == 0);

  token = clex(lexer);
  assert(token.kind == SEMICOL);
  assert(strcmp(token.lexeme, ";") == 0);

  clexReset(lexer, "auto$ ident1");
  token = clex(lexer);
  assert(token.kind == AUTO);
  assert(strcmp(token.lexeme, "auto") == 0);
  token = clex(lexer);
  assert(token.kind == CLEX_TOKEN_ERROR);
  assert(strcmp(token.lexeme, "$") == 0);
  free(token.lexeme);

  clexDeleteKinds(lexer);

  clexRegisterKind(lexer, "auto", AUTO);
  clexRegisterKind(lexer, "_Bool", BOOL);
  clexRegisterKind(lexer, "break", BREAK);
  clexRegisterKind(lexer, "case", CASE);
  clexRegisterKind(lexer, "char", CHAR);
  clexRegisterKind(lexer, "_Complex", COMPLEX);
  clexRegisterKind(lexer, "const", CONST);
  clexRegisterKind(lexer, "continue", CONTINUE);
  clexRegisterKind(lexer, "default", DEFAULT);
  clexRegisterKind(lexer, "do", DO);
  clexRegisterKind(lexer, "double", DOUBLE);
  clexRegisterKind(lexer, "else", ELSE);
  clexRegisterKind(lexer, "enum", ENUM);
  clexRegisterKind(lexer, "extern", EXTERN);
  clexRegisterKind(lexer, "float", FLOAT);
  clexRegisterKind(lexer, "for", FOR);
  clexRegisterKind(lexer, "goto", GOTO);
  clexRegisterKind(lexer, "if", IF);
  clexRegisterKind(lexer, "_Imaginary", IMAGINARY);
  clexRegisterKind(lexer, "inline", INLINE);
  clexRegisterKind(lexer, "int", INT);
  clexRegisterKind(lexer, "long", LONG);
  clexRegisterKind(lexer, "register", REGISTER);
  clexRegisterKind(lexer, "restrict", RESTRICT);
  clexRegisterKind(lexer, "return", RETURN);
  clexRegisterKind(lexer, "short", SHORT);
  clexRegisterKind(lexer, "signed", SIGNED);
  clexRegisterKind(lexer, "sizeof", SIZEOF);
  clexRegisterKind(lexer, "static", STATIC);
  clexRegisterKind(lexer, "struct", STRUCT);
  clexRegisterKind(lexer, "switch", SWITCH);
  clexRegisterKind(lexer, "typedef", TYPEDEF);
  clexRegisterKind(lexer, "union", UNION);
  clexRegisterKind(lexer, "unsigned", UNSIGNED);
  clexRegisterKind(lexer, "void", VOID);
  clexRegisterKind(lexer, "volatile", VOLATILE);
  clexRegisterKind(lexer, "while", WHILE);
  clexRegisterKind(lexer, "...", ELLIPSIS);
  clexRegisterKind(lexer, ">>=", RIGHT_ASSIGN);
  clexRegisterKind(lexer, "<<=", LEFT_ASSIGN);
  clexRegisterKind(lexer, "\\+=", ADD_ASSIGN);
  clexRegisterKind(lexer, "-=", SUB_ASSIGN);
  clexRegisterKind(lexer, "\\*=", MUL_ASSIGN);
  clexRegisterKind(lexer, "/=", DIV_ASSIGN);
  clexRegisterKind(lexer, "%=", MOD_ASSIGN);
  clexRegisterKind(lexer, "&=", AND_ASSIGN);
  clexRegisterKind(lexer, "^=", XOR_ASSIGN);
  clexRegisterKind(lexer, "\\|=", OR_ASSIGN);
  clexRegisterKind(lexer, ">>", RIGHT_OP);
  clexRegisterKind(lexer, "<<", LEFT_OP);
  clexRegisterKind(lexer, "\\+\\+", INC_OP);
  clexRegisterKind(lexer, "--", DEC_OP);
  clexRegisterKind(lexer, "->", PTR_OP);
  clexRegisterKind(lexer, "&&", AND_OP);
  clexRegisterKind(lexer, "\\|\\|", OR_OP);
  clexRegisterKind(lexer, "<=", LE_OP);
  clexRegisterKind(lexer, ">=", GE_OP);
  clexRegisterKind(lexer, "==", EQ_OP);
  clexRegisterKind(lexer, "!=", NE_OP);
  clexRegisterKind(lexer, ";", SEMICOL);
  clexRegisterKind(lexer, "{|<%", OCURLYBRACE);
  clexRegisterKind(lexer, "}|%>", CCURLYBRACE);
  clexRegisterKind(lexer, ",", COMMA);
  clexRegisterKind(lexer, ":", COLON);
  clexRegisterKind(lexer, "=", EQUAL);
  clexRegisterKind(lexer, "\\(", OPARAN);
  clexRegisterKind(lexer, "\\)", CPARAN);
  clexRegisterKind(lexer, "\\[|<:", OSQUAREBRACE);
  clexRegisterKind(lexer, "\\]|:>", CSQUAREBRACE);
  clexRegisterKind(lexer, ".", DOT);
  clexRegisterKind(lexer, "&", AMPER);
  clexRegisterKind(lexer, "!", EXCLAMATION);
  clexRegisterKind(lexer, "~", TILDE);
  clexRegisterKind(lexer, "-", MINUS);
  clexRegisterKind(lexer, "\\+", PLUS);
  clexRegisterKind(lexer, "\\*", STAR);
  clexRegisterKind(lexer, "/", SLASH);
  clexRegisterKind(lexer, "%", PERCENT);
  clexRegisterKind(lexer, "<", RANGLE);
  clexRegisterKind(lexer, ">", LANGLE);
  clexRegisterKind(lexer, "^", CARET);
  clexRegisterKind(lexer, "\\|", PIPE);
  clexRegisterKind(lexer, "\\?", QUESTION);
  clexRegisterKind(lexer, "L?\"[ -~]*\"", STRINGLITERAL);
  clexRegisterKind(lexer, "0[xX][a-fA-F0-9]+([uU])?([lL])?([lL])?", CONSTANT);
  clexRegisterKind(lexer, "0[0-7]*([uU])?([lL])?([lL])?", CONSTANT);
  clexRegisterKind(lexer, "[1-9][0-9]*([uU])?([lL])?([lL])?", CONSTANT);
  clexRegisterKind(lexer, "L?'[ -~]*'", CONSTANT);
  clexRegisterKind(lexer, "[0-9]+[Ee][+-]?[0-9]+[fFlL]", CONSTANT);
  clexRegisterKind(lexer, "[0-9]*.[0-9]+[Ee][+-]?[fFlL]", CONSTANT);
  clexRegisterKind(lexer, "[0-9]+.[0-9]*[Ee][+-]?[fFlL]", CONSTANT);
  clexRegisterKind(lexer, "0[xX][a-fA-F0-9]+[Pp][+-]?[0-9]+([fFlL])?",
                   CONSTANT);
  clexRegisterKind(lexer,
                   "0[xX][a-fA-F0-9]*.[a-fA-F0-9]+[Pp][+-]?[0-9]+([fFlL])?",
                   CONSTANT);
  clexRegisterKind(lexer,
                   "0[xX][a-fA-F0-9]+.[a-fA-F0-9]+[Pp][+-]?[0-9]+([fFlL])?",
                   CONSTANT);
  clexRegisterKind(lexer, "[a-zA-Z_]([a-zA-Z_]|[0-9])*", IDENTIFIER);
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

  clexReset(lexer, "int main(int argc, char *argv[]) {\nreturn 23;\n}");

  token = clex(lexer);
  assert(token.kind == INT);
  assert(strcmp(token.lexeme, "int") == 0);

  token = clex(lexer);
  assert(token.kind == IDENTIFIER);
  assert(strcmp(token.lexeme, "main") == 0);

  token = clex(lexer);
  assert(token.kind == OPARAN);
  assert(strcmp(token.lexeme, "(") == 0);

  token = clex(lexer);
  assert(token.kind == INT);
  assert(strcmp(token.lexeme, "int") == 0);

  token = clex(lexer);
  assert(token.kind == IDENTIFIER);
  assert(strcmp(token.lexeme, "argc") == 0);

  token = clex(lexer);
  assert(token.kind == COMMA);
  assert(strcmp(token.lexeme, ",") == 0);

  token = clex(lexer);
  assert(token.kind == CHAR);
  assert(strcmp(token.lexeme, "char") == 0);

  token = clex(lexer);
  assert(token.kind == STAR);
  assert(strcmp(token.lexeme, "*") == 0);

  token = clex(lexer);
  assert(token.kind == IDENTIFIER);
  assert(strcmp(token.lexeme, "argv") == 0);

  token = clex(lexer);
  assert(token.kind == OSQUAREBRACE);
  assert(strcmp(token.lexeme, "[") == 0);

  token = clex(lexer);
  assert(token.kind == CSQUAREBRACE);
  assert(strcmp(token.lexeme, "]") == 0);

  token = clex(lexer);
  assert(token.kind == CPARAN);
  assert(strcmp(token.lexeme, ")") == 0);

  token = clex(lexer);
  assert(token.kind == OCURLYBRACE);
  assert(strcmp(token.lexeme, "{") == 0);

  token = clex(lexer);
  assert(token.kind == RETURN);
  assert(strcmp(token.lexeme, "return") == 0);

  token = clex(lexer);
  assert(token.kind == CONSTANT);
  assert(strcmp(token.lexeme, "23") == 0);

  token = clex(lexer);
  assert(token.kind == SEMICOL);
  assert(strcmp(token.lexeme, ";") == 0);

  token = clex(lexer);
  assert(token.kind == CCURLYBRACE);
  assert(strcmp(token.lexeme, "}") == 0);

  token = clex(lexer);
  assert(token.kind == CLEX_TOKEN_EOF);
  assert(token.lexeme == NULL);
}
#endif

#ifdef TEST_REGEX

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "fa.h"

int main(void) {
  clexNode* nfa = clexNfaFromRe("a", NULL);
  assert(clexNfaTest(nfa, "a") == true);
  assert(clexNfaTest(nfa, "b") == false);

  nfa = clexNfaFromRe("ab", NULL);
  assert(clexNfaTest(nfa, "ab") == true);
  assert(clexNfaTest(nfa, "a") == false);
  assert(clexNfaTest(nfa, "b") == false);

  nfa = clexNfaFromRe("ab|c", NULL);
  assert(clexNfaTest(nfa, "ab") == true);
  assert(clexNfaTest(nfa, "c") == true);
  assert(clexNfaTest(nfa, "abc") == false);

  nfa = clexNfaFromRe("ab*c", NULL);
  assert(clexNfaTest(nfa, "c") == true);
  assert(clexNfaTest(nfa, "abc") == true);
  assert(clexNfaTest(nfa, "ababc") == true);
  assert(clexNfaTest(nfa, "ab") == false);
  assert(clexNfaTest(nfa, "abd") == false);
  assert(clexNfaTest(nfa, "acc") == false);

  nfa = clexNfaFromRe("ab+c", NULL);
  assert(clexNfaTest(nfa, "c") == false);
  assert(clexNfaTest(nfa, "abc") == true);
  assert(clexNfaTest(nfa, "ababc") == true);
  assert(clexNfaTest(nfa, "ab") == false);
  assert(clexNfaTest(nfa, "abd") == false);
  assert(clexNfaTest(nfa, "acc") == false);

  nfa = clexNfaFromRe("ab?c", NULL);
  assert(clexNfaTest(nfa, "c") == true);
  assert(clexNfaTest(nfa, "abc") == true);
  assert(clexNfaTest(nfa, "ababc") == false);
  assert(clexNfaTest(nfa, "ab") == false);
  assert(clexNfaTest(nfa, "abd") == false);
  assert(clexNfaTest(nfa, "acc") == false);

  nfa = clexNfaFromRe("[ab]c", NULL);
  assert(clexNfaTest(nfa, "c") == false);
  assert(clexNfaTest(nfa, "ac") == true);
  assert(clexNfaTest(nfa, "bc") == true);
  assert(clexNfaTest(nfa, "abc") == false);
  assert(clexNfaTest(nfa, "bd") == false);
  assert(clexNfaTest(nfa, "acc") == false);

  nfa = clexNfaFromRe("[A-Za-z]c", NULL);
  assert(clexNfaTest(nfa, "c") == false);
  assert(clexNfaTest(nfa, "ac") == true);
  assert(clexNfaTest(nfa, "bc") == true);
  assert(clexNfaTest(nfa, "Ac") == true);
  assert(clexNfaTest(nfa, "Zd") == false);
  assert(clexNfaTest(nfa, "Zc") == true);

  nfa = clexNfaFromRe("[A-Za-z]*c", NULL);
  assert(clexNfaTest(nfa, "AZazc") == true);
  assert(clexNfaTest(nfa, "AZaz") == false);

  nfa = clexNfaFromRe("[A-Za-z]?c", NULL);
  assert(clexNfaTest(nfa, "Ac") == true);
  assert(clexNfaTest(nfa, "c") == true);
  assert(clexNfaTest(nfa, "A") == false);

  nfa = clexNfaFromRe("a(bc|de)f", NULL);
  assert(clexNfaTest(nfa, "abcf") == true);
  assert(clexNfaTest(nfa, "adef") == true);
  assert(clexNfaTest(nfa, "af") == false);
  assert(clexNfaTest(nfa, "abf") == false);
  assert(clexNfaTest(nfa, "abcdef") == false);
  assert(clexNfaTest(nfa, "abccf") == false);
  assert(clexNfaTest(nfa, "bcf") == false);
  assert(clexNfaTest(nfa, "abc") == false);

  nfa = clexNfaFromRe("(bc|de)f", NULL);
  assert(clexNfaTest(nfa, "bcf") == true);
  assert(clexNfaTest(nfa, "def") == true);

  nfa = clexNfaFromRe("a(bc)*f", NULL);
  assert(clexNfaTest(nfa, "af") == true);
  assert(clexNfaTest(nfa, "abcf") == true);
  assert(clexNfaTest(nfa, "abcbcf") == true);
  assert(clexNfaTest(nfa, "abcbf") == false);

  nfa = clexNfaFromRe("(bc)*f", NULL);
  assert(clexNfaTest(nfa, "f") == true);
  assert(clexNfaTest(nfa, "bcf") == true);
  assert(clexNfaTest(nfa, "bcbcf") == true);
  assert(clexNfaTest(nfa, "bcbf") == false);
  assert(clexNfaTest(nfa, "bc") == false);

  nfa = clexNfaFromRe("a(bc|de)*f", NULL);
  assert(clexNfaTest(nfa, "af") == true);
  assert(clexNfaTest(nfa, "abcf") == true);
  assert(clexNfaTest(nfa, "adef") == true);
  assert(clexNfaTest(nfa, "abcbcf") == true);
  assert(clexNfaTest(nfa, "adedef") == true);
  assert(clexNfaTest(nfa, "abcdef") == true);
  assert(clexNfaTest(nfa, "abf") == false);
  assert(clexNfaTest(nfa, "abccf") == false);
  assert(clexNfaTest(nfa, "bcf") == false);
  assert(clexNfaTest(nfa, "abc") == false);

  nfa = clexNfaFromRe("a(bc|de)+f", NULL);
  assert(clexNfaTest(nfa, "af") == false);
  assert(clexNfaTest(nfa, "abcf") == true);
  assert(clexNfaTest(nfa, "adef") == true);
  assert(clexNfaTest(nfa, "abcbcf") == true);
  assert(clexNfaTest(nfa, "adedef") == true);
  assert(clexNfaTest(nfa, "abcdef") == true);
  assert(clexNfaTest(nfa, "abf") == false);
  assert(clexNfaTest(nfa, "abccf") == false);
  assert(clexNfaTest(nfa, "bcf") == false);
  assert(clexNfaTest(nfa, "abc") == false);

  nfa = clexNfaFromRe("a(bc|de)?f", NULL);
  assert(clexNfaTest(nfa, "af") == true);
  assert(clexNfaTest(nfa, "abcf") == true);
  assert(clexNfaTest(nfa, "adef") == true);
  assert(clexNfaTest(nfa, "abcbcf") == false);
  assert(clexNfaTest(nfa, "adedef") == false);
  assert(clexNfaTest(nfa, "abcdef") == false);
  assert(clexNfaTest(nfa, "abf") == false);
  assert(clexNfaTest(nfa, "abccf") == false);
  assert(clexNfaTest(nfa, "bcf") == false);
  assert(clexNfaTest(nfa, "abc") == false);

  nfa = clexNfaFromRe("([a-zA-Z_])*", NULL);
  assert(clexNfaTest(nfa, "valid") == true);
  assert(clexNfaTest(nfa, "Valid") == true);
  assert(clexNfaTest(nfa, "_var1") == false);
  assert(clexNfaTest(nfa, "vv1") == false);
  assert(clexNfaTest(nfa, "v1") == false);

  nfa = clexNfaFromRe("([a-zA-Z_]|[0-9])*", NULL);
  assert(clexNfaTest(nfa, "valid") == true);
  assert(clexNfaTest(nfa, "Valid") == true);
  assert(clexNfaTest(nfa, "_var1") == true);
  assert(clexNfaTest(nfa, "vv1") == true);
  assert(clexNfaTest(nfa, "v1") == true);

  nfa = clexNfaFromRe("[a-zA-Z_]([a-zA-Z_]|[0-9])*", NULL);
  assert(clexNfaTest(nfa, "valid") == true);
  assert(clexNfaTest(nfa, "Valid") == true);
  assert(clexNfaTest(nfa, "_var1") == true);
  assert(clexNfaTest(nfa, "vv1") == true);
  assert(clexNfaTest(nfa, "v1") == true);
  assert(clexNfaTest(nfa, "_") == true);
  assert(clexNfaTest(nfa, "_1") == true);
  assert(clexNfaTest(nfa, "a") == true);
  assert(clexNfaTest(nfa, "23a") == false);

  nfa = clexNfaFromRe("\\|", NULL);
  assert(clexNfaTest(nfa, "|") == true);

  nfa = clexNfaFromRe("\\[", NULL);
  assert(clexNfaTest(nfa, "[") == true);

  nfa = clexNfaFromRe("\\]", NULL);
  assert(clexNfaTest(nfa, "]") == true);

  nfa = clexNfaFromRe("\\+", NULL);
  assert(clexNfaTest(nfa, "+") == true);

  nfa = clexNfaFromRe("\\*", NULL);
  assert(clexNfaTest(nfa, "*") == true);

  nfa = clexNfaFromRe("\\?", NULL);
  assert(clexNfaTest(nfa, "?") == true);

  nfa = clexNfaFromRe("(u)(a)", NULL);
  assert(clexNfaTest(nfa, "ua") == true);

  nfa = clexNfaFromRe("[1-9][0-9]*([uU])?([lL])?([lL])?", NULL);
  assert(clexNfaTest(nfa, "23") == true);
  assert(clexNfaTest(nfa, "23u") == true);
  assert(clexNfaTest(nfa, "23UlL") == true);

  nfa = clexNfaFromRe("[", NULL);
  assert(nfa == 0);
  nfa = clexNfaFromRe("\\", NULL);
  assert(nfa == 0);
  nfa = clexNfaFromRe("(", NULL);
  assert(nfa == 0);
  nfa = clexNfaFromRe("a(", NULL);
  assert(nfa == 0);
  nfa = clexNfaFromRe("a|", NULL);
  assert(nfa == 0);
  nfa = clexNfaFromRe("|", NULL);
  assert(nfa == 0);
  nfa = clexNfaFromRe("a)", NULL);
  assert(nfa == 0);

  char longClassRe[160];
  longClassRe[0] = '[';
  memset(longClassRe + 1, 'a', 150);
  longClassRe[151] = ']';
  longClassRe[152] = '\0';
  nfa = clexNfaFromRe(longClassRe, NULL);
  assert(nfa == 0);

  size_t longLen = 1100;
  const char* suffix = "(bc)*";
  char* longRegex = malloc(longLen + strlen(suffix) + 1);
  assert(longRegex != NULL);
  memset(longRegex, 'a', longLen);
  memcpy(longRegex + longLen, suffix, strlen(suffix) + 1);
  nfa = clexNfaFromRe(longRegex, NULL);
  assert(nfa != NULL);
  clexNfaDestroy(nfa, NULL);
  free(longRegex);
}
#endif

#ifdef TEST_NFA_DRAW

#include "fa.h"

int main(void) {
  clexNode* nfa = clexNfaFromRe("[a-zA-Z_]([a-zA-Z_]|[0-9])*", NULL);
  clexNfaDraw(nfa);

  nfa = clexNfaFromRe("[A-Z]a(bc|de)*f", NULL);
  clexNfaDraw(nfa);
}
#endif
