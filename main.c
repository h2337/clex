#ifdef TEST

#include "fa.h"
#include <assert.h>

int main(int argc, char *argv) {
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
}
#endif
