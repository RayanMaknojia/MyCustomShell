#include "lexer.h"
#include <stdio.h>
#include <string.h>

static const char *names[] = {"WORD", "PIPE", "IN", "OUT", "APPEND", "SEMI", "AND", "OR", "BG"};

int main(void) {
  char line[4096];
  while (fgets(line, sizeof line, stdin) != NULL) {
    line[strcspn(line, "\n")] = '\0';
    token *toks;
    int n;
    if (lex(line, &toks, &n) != LEX_OK) {
      puts("ERROR");
      continue;
    }
    for (int i = 0; i < n; i++) {
      if (toks[i].type == TOK_WORD)
        printf("WORD[%s] ", toks[i].text);
      else
        printf("%s ", names[toks[i].type]);
    }
    putchar('\n');
    tokens_free(toks, n);
  }
  return 0;
}
