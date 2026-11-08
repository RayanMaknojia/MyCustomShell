#include "cd.h"
#include "msgs.h"
#include "util.h"
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static char *prev_dir = NULL;
void do_cd(int argc, char *argv[]) {
  if (argc > 2) {
    print_error("cd", TMA_MSG);
    return;
  }
  bool print_target = false;
  const char *target = NULL;
  char *to_free = NULL;

  if (argc == 1 || strcmp(argv[1], "~") == 0 || strncmp(argv[1], "~/", 2) == 0) {
    struct passwd *pw = getpwuid(getuid());
    if (pw == NULL || pw->pw_dir == NULL) {
      print_error("cd", CHDIR_ERROR_MSG);
      return;
    }
    if (argc == 1) {
      target = pw->pw_dir;
    } else {
      size_t size = strlen(pw->pw_dir) + strlen(argv[1]);
      to_free = malloc(size);
      if (to_free == NULL) {
        print_error("cd", CHDIR_ERROR_MSG);
        return;
      }
      snprintf(to_free, size, "%s%s", pw->pw_dir, argv[1] + 1);
      target = to_free;
    }
  } else if (strcmp(argv[1], "-") == 0) {
    if (prev_dir == NULL) {
      print_error("cd", CHDIR_ERROR_MSG);
      return;
    }
    target = prev_dir;
    print_target = true;
  } else {
    target = argv[1];
  }

  char *old = getcwd(NULL, 0);
  if (chdir(target) == -1) {
    print_error("cd", CHDIR_ERROR_MSG);
    free(old);
  } else {
    if (print_target) {
      printf("%s\n", target);
    }
    if (old != NULL) {
      free(prev_dir);
      prev_dir = old;
    }
  }
  free(to_free);
}

void cd_free(void) {
  free(prev_dir);
  prev_dir = NULL;
}
