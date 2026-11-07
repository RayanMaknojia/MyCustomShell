#include "execute.h"
#include "msgs.h"
#include "util.h"
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void reap_zombies(void) {
  while (waitpid(-1, NULL, WNOHANG) > 0) {
  }
}

void run(char *argv[], bool background) {
  fflush(stderr);
  fflush(stdout);

  pid_t pid = fork();
  if (pid < 0) {
    print_error("shell", FORK_ERROR_MSG);
    return;
  }
  if (pid == 0) {
    execvp(argv[0], argv);
    print_error("shell", EXEC_ERROR_MSG);
    _exit(1);
  }

  if (!background) {
    while (waitpid(pid, NULL, 0) == -1) {
      if (errno != EINTR) {
        print_error("shell", WAIT_ERROR_MSG);
        break;
      }
    }
  }
}
