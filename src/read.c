#define _POSIX_C_SOURCE 200809L
#include "read.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define INBUF_SIZE 4096

static char inbuf[INBUF_SIZE];
static size_t inlen = 0;

static ssize_t take_line(char *out, size_t out_size, size_t line_len, size_t consume) {
  size_t copy = line_len < out_size - 1 ? line_len : out_size - 1;
  memcpy(out, inbuf, copy);
  out[copy] = '\0';
  memmove(inbuf, inbuf + consume, inlen - consume);
  inlen -= consume;
  return (ssize_t)consume;
}

ssize_t read_command(char *out, size_t out_size) {
  while (1) {
    char *nl = memchr(inbuf, '\n', inlen);
    if (nl != NULL) {
      size_t len = (size_t)(nl - inbuf);
      return take_line(out, out_size, len, len + 1);
    }
    if (inlen == INBUF_SIZE) { // buffer full, no newline: treat as one long line
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
      return take_line(out, out_size, inlen, inlen); // leftover with no trailing \n
    }
    inlen += (size_t)n;
  }
}

int tokenize(char *buf, char *argv[], bool *background) {
  int argc = 0;
  char *saveptr = NULL;
  *background = false;

  char *token = strtok_r(buf, " \t\n", &saveptr);
  while (token != NULL && argc < MAX_ARGS) {
    argv[argc++] = token;
    token = strtok_r(NULL, " \t\n", &saveptr);
  }
  argv[argc] = NULL;

  if (argc == 0) {
    return 0;
  }

  if (strcmp(argv[argc - 1], "&") == 0) {
    *background = true;
    argv[--argc] = NULL;
  } else {
    size_t len = strlen(argv[argc - 1]);
    if (argv[argc - 1][len - 1] == '&') {
      *background = true;
      argv[argc - 1][len - 1] = '\0';
    }
  }
  return argc;
}
