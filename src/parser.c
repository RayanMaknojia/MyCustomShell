#define _POSIX_C_SOURCE 200809L
#include "parser.h"
#include <stdlib.h>
#include <string.h>

static void command_free(command *c) {
  for (int i = 0; i < c->argc; i++) {
    free(c->argv[i]);
  }
  free(c->argv);
  free(c->in_file);
  free(c->out_file);
}

void pipeline_free(pipeline *p) {
  for (int i = 0; i < p->ncmds; i++) {
    command_free(&p->cmds[i]);
  }
  free(p->cmds);
  memset(p, 0, sizeof *p);
}

// Append a copy of text to c->argv, keeping argv NULL-terminated.
static int add_word(command *c, const char *text) {
  char *copy = strdup(text);
  if (copy == NULL) {
    return PARSE_NOMEM;
  }
  char **grown = realloc(c->argv, (size_t)(c->argc + 2) * sizeof(char *));
  if (grown == NULL) {
    free(copy);
    return PARSE_NOMEM;
  }
  c->argv = grown;
  c->argv[c->argc++] = copy;
  c->argv[c->argc] = NULL;
  return PARSE_OK;
}

int parse(const token *toks, int ntoks, pipeline *out) {
  memset(out, 0, sizeof *out);
  if (ntoks == 0) {
    return PARSE_EMPTY;
  }

  // One command per pipe-separated segment.
  int ncmds = 1;
  for (int i = 0; i < ntoks; i++) {
    if (toks[i].type == TOK_PIPE) {
      ncmds++;
    }
  }
  out->cmds = calloc((size_t)ncmds, sizeof(command)); // zeroed, so safe to free midway
  if (out->cmds == NULL) {
    return PARSE_NOMEM;
  }
  out->ncmds = ncmds;

  int rc = PARSE_OK;
  int ci = 0; // index of the command being filled
  for (int i = 0; i < ntoks && rc == PARSE_OK; i++) {
    command *c = &out->cmds[ci];
    switch (toks[i].type) {
    case TOK_WORD:
      rc = add_word(c, toks[i].text);
      break;
    case TOK_REDIR_IN:
    case TOK_REDIR_OUT:
    case TOK_REDIR_APPEND: {
      // A redirection must be followed by a file name.
      if (i + 1 >= ntoks || toks[i + 1].type != TOK_WORD) {
        rc = PARSE_SYNTAX;
        break;
      }
      char *name = strdup(toks[i + 1].text);
      if (name == NULL) {
        rc = PARSE_NOMEM;
        break;
      }
      if (toks[i].type == TOK_REDIR_IN) {
        free(c->in_file); // a later redirection replaces an earlier one
        c->in_file = name;
      } else {
        free(c->out_file);
        c->out_file = name;
        c->append = (toks[i].type == TOK_REDIR_APPEND);
      }
      i++; // we consumed the file name too
      break;
    }
    case TOK_PIPE:
      if (c->argc == 0) { // nothing before the pipe
        rc = PARSE_SYNTAX;
        break;
      }
      ci++;
      break;
    case TOK_BG:
      // Only valid as the very last token, after a real command.
      if (i != ntoks - 1 || c->argc == 0) {
        rc = PARSE_SYNTAX;
      } else {
        out->background = true;
      }
      break;
    default: // ; && || are not supported yet
      rc = PARSE_SYNTAX;
      break;
    }
  }

  // The last command needs a program name (this catches "a |" and "> f").
  if (rc == PARSE_OK && out->cmds[ncmds - 1].argc == 0) {
    rc = PARSE_SYNTAX;
  }
  if (rc != PARSE_OK) {
    pipeline_free(out);
  }
  return rc;
}
