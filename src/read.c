#define _POSIX_C_SOURCE 200809L
#include "read.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

ssize_t read_command(char *buf, size_t size) {
  ssize_t n = read(STDIN_FILENO, buf, size - 1);
  if (n <= 0) {
    return n;
  }
  buf[n] = '\0';
  if (buf[n - 1] == '\n') {
    buf[n - 1] = '\0';
  }
  return n;
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
