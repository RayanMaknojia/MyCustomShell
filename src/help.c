#include "help.h"
#include "msgs.h"
#include "stdio.h"
#include "string.h"
#include "util.h"
#include <unistd.h>

struct entry {
  const char *name;
  const char *msg;
};
static const struct entry table[] = {{"exit", EXIT_HELP_MSG},
                                     {"pwd", PWD_HELP_MSG},
                                     {"cd", CD_HELP_MSG},
                                     {"help", HELP_HELP_MSG},
                                     {"history", HISTORY_HELP_MSG}};
static const int count = sizeof(table) / sizeof(table[0]);

void do_help(int argc, char *argv[]) {
  if (argc > 2) {
    print_error("help", TMA_MSG);
    return;
  }
  if (argc == 2) {
    for (int i = 0; i < count; i++) {
      if (strcmp(argv[1], table[i].name) == 0) {
        printf(FORMAT_MSG, table[i].name, table[i].msg);
        return;
      }
    }
    printf(FORMAT_MSG, argv[1], EXTERN_HELP_MSG);
    return;
  }
  for (int i = 0; i < count; i++) {
    printf(FORMAT_MSG, table[i].name, table[i].msg);
  }
}

void help_signal(void) {
  for (int i = 0; i < count; i++) {
    write(STDOUT_FILENO, table[i].name, strlen(table[i].name));
    write(STDOUT_FILENO, ": ", 2);
    write(STDOUT_FILENO, table[i].msg, strlen(table[i].msg));
    write(STDOUT_FILENO, "\n", 1);
  }
}
