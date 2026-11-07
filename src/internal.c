#include "internal.h"
#include "cd.h"
#include "help.h"
#include "history.h"
#include "msgs.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_internal(int argc, char *argv[]) {
  if (strcmp(argv[0], "exit") == 0) {
    if (argc > 1) {
      print_error("exit", TMA_MSG);
      return 1;
    }
    return 2;
  }
  if (strcmp(argv[0], "pwd") == 0) {
    if (argc > 1) {
      print_error("pwd", TMA_MSG);
      return 1;
    }
    char *cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
      print_error("pwd", GETCWD_ERROR_MSG);
      return 1;
    }
    printf("%s\n", cwd);
    free(cwd);
    return 1;
  }
  if (strcmp(argv[0], "help") == 0) {
    do_help(argc, argv);
    return 1;
  }
  if (strcmp(argv[0], "cd") == 0) {
    do_cd(argc, argv);
    return 1;
  }
  if (strcmp(argv[0], "history") == 0) {
    history_print();
    return 1;
  }

  return 0;
}
