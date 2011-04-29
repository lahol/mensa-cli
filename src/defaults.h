#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

int defaults_read(unsigned char *filename);
int defaults_write(unsigned char *filename);
void defaults_free(void);

void defaults_get(unsigned char *key, unsigned char *value, int len);

void defaults_add(unsigned char *key, unsigned char *value);
void defaults_update(unsigned char *key, unsigned char *value);

#endif
