#ifndef READ_H
#define READ_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#define MAX_ARGS 64

ssize_t read_command(char *buf, size_t size);

int tokenize(char *buf, char *argv[], bool *background);

#endif
