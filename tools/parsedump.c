#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

int main(void) {
  char line[4096];
  while (fgets(line, sizeof line, stdin) != NULL) {
    line[strcspn(line, "\n")] = '\0';
    token *toks;
    int n;
    if (lex(line, &toks, &n) != LEX_OK) {
      puts("LEX ERROR");
      continue;
    }
    pipeline p;
    int rc = parse(toks, n, &p);
    tokens_free(toks, n);
    if (rc == PARSE_EMPTY) {
      puts("EMPTY");
      continue;
    }
    if (rc != PARSE_OK) {
      puts(rc == PARSE_SYNTAX ? "SYNTAX ERROR" : "NOMEM");
      continue;
    }
    for (int i = 0; i < p.ncmds; i++) {
      command *c = &p.cmds[i];
      if (i > 0)
        printf(" | ");
      for (int j = 0; j < c->argc; j++) {
        if (j > 0)
          putchar(' ');
        printf("[%s]", c->argv[j]);
      }
      if (c->in_file)
        printf(" <%s", c->in_file);
      if (c->out_file)
        printf(" %s%s", c->append ? ">>" : ">", c->out_file);
    }
    if (p.background)
      printf(" &");
    putchar('\n');
    pipeline_free(&p);
  }
  return 0;
}
