#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdbool.h>

void reap_zombies(void);
void run(char *argv[], bool background);

#endif
