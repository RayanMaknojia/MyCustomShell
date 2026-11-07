#ifndef HISTORY_H
#define HISTORY_H

void history_add(const char *line);
void history_print(void);
void history_free(void);
const char *history_get(int n);
const char *history_last(void);

#endif
