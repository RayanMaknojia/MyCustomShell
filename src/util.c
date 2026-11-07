#include "util.h"
#include "msgs.h"
#include <stdio.h>

void print_error(const char *com, const char *msg) { fprintf(stderr, FORMAT_MSG, com, msg); }
