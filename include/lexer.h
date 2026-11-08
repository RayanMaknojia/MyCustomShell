#ifndef LEXER_H
#define LEXER_H

typedef enum {
  TOK_WORD,
  TOK_PIPE,         // |
  TOK_REDIR_IN,     // <
  TOK_REDIR_OUT,    // >
  TOK_REDIR_APPEND, // >>
  TOK_SEMI,         // ;
  TOK_AND,          // &&
  TOK_OR,           // ||
  TOK_BG            // &
} token_type;

typedef struct {
  token_type type;
  char *text;
} token;

#define LEX_OK 0
#define LEX_UNTERMINATED -1 // quote was never closed
#define LEX_NOMEM -2

// Splits line into tokens. On LEX_OK, *out is a heap array of *count tokens
// (free it with tokens_free). On error, *out is NULL and *count is 0.
int lex(const char *line, token **out, int *count);

void tokens_free(token *toks, int count);

#endif
