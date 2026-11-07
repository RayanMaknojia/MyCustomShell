#define _POSIX_C_SOURCE 200809L
#include "cd.h"
#include "execute.h"
#include "help.h"
#include "history.h"
#include "internal.h"
#include "msgs.h"
#include "read.h"
#include "util.h"
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void handle_sigint(int sig) {
  (void)sig;
  help_signal();
}

int main(void) {
  char buf[2048];
  char line[2048];
  char *argv[MAX_ARGS + 1];
  bool background;

  struct sigaction sa;
  sa.sa_handler = handle_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);

  while (1) {
    char *cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
      print_error("shell", GETCWD_ERROR_MSG);
    } else {
      printf("%s$ ", cwd);
      fflush(stdout);
      free(cwd);
    }

    ssize_t n = read_command(buf, sizeof(buf));

    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      print_error("shell", READ_ERROR_MSG);
      break;
    }
    if (n == 0) {
      printf("\n");
      break;
    }

    reap_zombies();
    char *p = buf;
    while (*p == ' ' || *p == '\t') {
      p++;
    }
    size_t plen = strlen(p);
    while (plen > 0 && (p[plen - 1] == ' ' || p[plen - 1] == '\t')) {
      p[--plen] = '\0';
    }
    if (p[0] == '!') {
      const char *cmd = NULL;

      if (strcmp(p, "!!") == 0) {
        cmd = history_last();
        if (cmd == NULL) {
          print_error("history", HISTORY_NO_LAST_MSG);
          continue;
        }
      } else {
        char *end;
        long num = strtol(p + 1, &end, 10);
        if (end == p + 1 || *end != '\0') {
          print_error("history", HISTORY_INVALID_MSG);
          continue;
        }
        cmd = history_get((int)num);
        if (cmd == NULL) {
          print_error("history", HISTORY_INVALID_MSG);
          continue;
        }
      }
      snprintf(buf, sizeof(buf), "%s", cmd);
      printf("%s\n", buf);
      fflush(stdout);
    }
    snprintf(line, sizeof(line), "%s", buf);

    int argc = tokenize(buf, argv, &background);
    if (argc == 0) {
      continue;
    }
    history_add(line);

    int status = run_internal(argc, argv);
    if (status == 2) {
      break;
    } else if (status == 1) {
      continue;
    } else {
      run(argv, background);
    }
  }
  cd_free();
  history_free();
  return 0;
}
