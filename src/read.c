#define _POSIX_C_SOURCE 200809L
#include "read.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define INBUF_SIZE 4096

static char inbuf[INBUF_SIZE];
static size_t inlen = 0; // number of valid bytes in inbuf

// Copy the first line_len bytes of inbuf into out (truncating if out is too
// small), then drop `consume` bytes from the front of inbuf. consume is
// line_len + 1 when a newline is being skipped.
static ssize_t take_line(char *out, size_t out_size, size_t line_len, size_t consume) {
  size_t copy = line_len < out_size - 1 ? line_len : out_size - 1;
  memcpy(out, inbuf, copy);
  out[copy] = '\0';
  memmove(inbuf, inbuf + consume, inlen - consume);
  inlen -= consume;
  return (ssize_t)consume;
}

// Returns > 0 when a line was stored in out ,
// 0 at EOF, or -1 on error with errno set. EINTR is passed
ssize_t read_command(char *out, size_t out_size) {
  while (1) {
    // for lines waiting from earlier
    char *nl = memchr(inbuf, '\n', inlen);
    if (nl != NULL) {
      size_t len = (size_t)(nl - inbuf);
      return take_line(out, out_size, len, len + 1);
    }
    // if the line is entire buffer
    if (inlen == INBUF_SIZE) {
      return take_line(out, out_size, inlen, inlen);
    }
    ssize_t n = read(STDIN_FILENO, inbuf + inlen, INBUF_SIZE - inlen);
    if (n < 0) {
      return -1;
    }
    if (n == 0) { // EOF
      if (inlen == 0) {
        return 0;
      }
      return take_line(out, out_size, inlen, inlen);
    }
    inlen += (size_t)n;
  }
}

// Splits a command line into argv[] in place, with shell-style quoting:
//   "..."  grouped into one argument, quotes removed; \" and \\ are escapes
//   '...'  grouped into one argument, everything literal
//   \c     outside quotes, makes the next character literal
// A trailing unquoted '&' marks a background command.
int tokenize(char *buf, char *argv[], bool *background) {
  int argc = 0;
  char *r = buf;         // read position
  char *w = buf;         // write position
  bool last_amp = false; // check for '&'
  *background = false;

  while (1) {
    while (*r == ' ' || *r == '\t' || *r == '\n') {
      r++;
    }
    if (*r == '\0' || argc >= MAX_ARGS) {
      break;
    }

    argv[argc++] = w;
    last_amp = false;
    char quote = 0; // 0 = not in quotes, or ' or "

    while (*r != '\0') {
      char c = *r;
      if (quote == 0) {
        if (c == ' ' || c == '\t' || c == '\n') {
          break;
        }
        if (c == '\'' || c == '"') {
          quote = c; // open a quoted section with quotes
          r++;
        } else if (c == '\\' && r[1] != '\0') {
          r++; // backslash: take the next character literally
          *w++ = *r++;
          last_amp = false;
        } else {
          *w++ = c;
          last_amp = (c == '&');
          r++;
        }
      } else if (quote == '\'') {
        // Single quotes: everything is literal until the closing quote.
        if (c == '\'') {
          quote = 0;
        } else {
          *w++ = c;
          last_amp = false;
        }
        r++;
      } else {
        // Double quotes: literal, except \" and \\ are escapes.
        if (c == '"') {
          quote = 0;
          r++;
        } else if (c == '\\' && (r[1] == '"' || r[1] == '\\')) {
          r++;
          *w++ = *r++;
          last_amp = false;
        } else {
          *w++ = c;
          last_amp = false;
          r++;
        }
      }
    }

    bool at_end = (*r == '\0');
    *w++ = '\0';
    if (!at_end) {
      r++;
    }
  }
  argv[argc] = NULL;

  // Background check: & but only if not in any quotes ,
  if (argc > 0 && last_amp) {
    *background = true;
    char *last = argv[argc - 1];
    last[strlen(last) - 1] = '\0';
    if (last[0] == '\0') {
      argv[--argc] = NULL;
    }
  }
  return argc;
}
