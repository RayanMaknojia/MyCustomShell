#define _POSIX_C_SOURCE 200809L
#include "lexer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static bool is_space(char c) { return c == ' ' || c == '\t' || c == '\n'; }

static bool is_op_char(char c) { return c == '|' || c == '<' || c == '>' || c == ';' || c == '&'; }

// If an operator starts at r, store its type and return its length (1 or 2). Otherwise return 0.
static int match_operator(const char *r, token_type *type) {
  if (r[0] == '&' && r[1] == '&') {
    *type = TOK_AND;
    return 2;
  }
  if (r[0] == '|' && r[1] == '|') {
    *type = TOK_OR;
    return 2;
  }
  if (r[0] == '>' && r[1] == '>') {
    *type = TOK_REDIR_APPEND;
    return 2;
  }
  switch (r[0]) {
  case '|':
    *type = TOK_PIPE;
    return 1;
  case '<':
    *type = TOK_REDIR_IN;
    return 1;
  case '>':
    *type = TOK_REDIR_OUT;
    return 1;
  case ';':
    *type = TOK_SEMI;
    return 1;
  case '&':
    *type = TOK_BG;
    return 1;
  default:
    return 0;
  }
}

// Append a token, growing the array as needed. it is freed here if the append fails.
static int push(token **toks, int *count, int *cap, token_type type, char *text) {
  if (*count == *cap) {
    int ncap = *cap ? *cap * 2 : 16;
    token *grown = realloc(*toks, (size_t)ncap * sizeof(token));
    if (grown == NULL) {
      free(text);
      return LEX_NOMEM;
    }
    *toks = grown;
    *cap = ncap;
  }
  (*toks)[*count].type = type;
  (*toks)[*count].text = text;
  (*count)++;
  return LEX_OK;
}

void tokens_free(token *toks, int count) {
  for (int i = 0; i < count; i++) {
    free(toks[i].text);
  }
  free(toks);
}

int lex(const char *line, token **out, int *count_out) {
  token *toks = NULL;
  int count = 0, cap = 0;
  int rc = LEX_OK;

  // A word can never be longer than the whole line, so one scratch buffer
  // is enough for building each word.
  char *scratch = malloc(strlen(line) + 1);
  if (scratch == NULL) {
    *out = NULL;
    *count_out = 0;
    return LEX_NOMEM;
  }

  const char *r = line;
  while (*r != '\0') {
    if (is_space(*r)) {
      r++;
      continue;
    }

    token_type type;
    int oplen = match_operator(r, &type);
    if (oplen > 0) {
      rc = push(&toks, &count, &cap, type, NULL);
      if (rc != LEX_OK)
        break;
      r += oplen;
      continue;
    }

    // Otherwise this starts a word. an unquoted operator character ends the word .
    size_t w = 0;
    char quote = 0; // 0 = unquoted, otherwise the opening ' or "
    while (*r != '\0') {
      char c = *r;
      if (quote == 0) {
        if (is_space(c) || is_op_char(c))
          break;
        if (c == '\'' || c == '"') {
          quote = c;
          r++;
        } else if (c == '\\' && r[1] != '\0') {
          r++;
          scratch[w++] = *r++;
        } else {
          scratch[w++] = c;
          r++;
        }
      } else if (quote == '\'') {
        if (c == '\'')
          quote = 0;
        else
          scratch[w++] = c;
        r++;
      } else { // inside "..."
        if (c == '"') {
          quote = 0;
          r++;
        } else if (c == '\\' && (r[1] == '"' || r[1] == '\\')) {
          r++;
          scratch[w++] = *r++;
        } else {
          scratch[w++] = c;
          r++;
        }
      }
    }
    if (quote != 0) {
      rc = LEX_UNTERMINATED;
      break;
    }
    scratch[w] = '\0';
    char *text = strdup(scratch);
    if (text == NULL) {
      rc = LEX_NOMEM;
      break;
    }
    rc = push(&toks, &count, &cap, TOK_WORD, text);
    if (rc != LEX_OK)
      break;
  }

  free(scratch);
  if (rc != LEX_OK) {
    tokens_free(toks, count);
    *out = NULL;
    *count_out = 0;
    return rc;
  }
  *out = toks;
  *count_out = count;
  return LEX_OK;
}
