#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

typedef enum {
  DEFAULTS_ERROR_OK = 0,
  DEFAULTS_ERROR_TYPE_MISMATCH,
  DEFAULTS_ERROR_NOTFOUND,
  DEFAULTS_ERROR_NOTENOUGHSPACE,
} DefaultsError;

int defaults_read(unsigned char *filename);
int defaults_write(unsigned char *filename);
void defaults_free(void);

DefaultsError defaults_get(unsigned char *key, unsigned char *value, int len);
DefaultsError defaults_get_int(unsigned char *key, int *value);
DefaultsError defaults_get_boolean(unsigned char *key, int *value);

void defaults_add(unsigned char *key, unsigned char *value);
void defaults_update(unsigned char *key, unsigned char *value);

#endif
