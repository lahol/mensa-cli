#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

int defaults_read(char *filename);
void defaults_get(char *key, char *value);

void defaults_add(char *key, char *value);

#endif