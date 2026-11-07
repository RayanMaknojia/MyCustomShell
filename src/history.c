#define _POSIX_C_SOURCE 200809L
#include "history.h"
#include "msgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *entries[10];
static int next_in_line = 0;

void history_add(const char *line) {
  free(entries[next_in_line % 10]);
  entries[next_in_line % 10] = strdup(line);
  next_in_line++;
}

void history_print(void) {
  int oldest = next_in_line - 10;
  if (oldest < 0) {
    oldest = 0;
  }
  for (int i = next_in_line - 1; i >= oldest; i--) {
    printf(FORMAT_HISTORY, i, entries[i % 10]);
  }
}

void history_free(void) {
  for (int i = 0; i < 10; i++) {
    free(entries[i]);
    entries[i] = NULL;
  }
}

const char *history_get(int n) {
  if (n < 0 || n >= next_in_line || n < next_in_line - 10) {
    return NULL;
  }
  return entries[n % 10];
}

const char *history_last(void) { return history_get(next_in_line - 1); }
