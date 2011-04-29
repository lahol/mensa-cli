#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

int defaults_read(char *filename);
int defaults_write(char *filename);
void defaults_free(void);

void defaults_get(char *key, char *value, int len);

void defaults_add(char *key, char *value);
void defaults_update(char *key, char *value);

#endif