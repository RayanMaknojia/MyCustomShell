#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdbool.h>

typedef struct {
  char **argv;
  int argc;
  char *in_file;
  char *out_file;
  bool append;
} command;

typedef struct {
  command *cmds; // array of ncmds commands, connected by pipes
  int ncmds;
  bool background; // trailing & after the last command
} pipeline;

#define PARSE_OK 0
#define PARSE_EMPTY 1
#define PARSE_SYNTAX -1
#define PARSE_NOMEM -2

int parse(const token *toks, int ntoks, pipeline *out);

void pipeline_free(pipeline *p);

#endif
