/** @defgroup defaults Default settings and configuration.
 *  @ingroup mensacli
 *  @file
 *  @ingroup defaults
 *  Header file for default settings and configuration.
 */
#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

/** @enum DefaultsError
 *  Error codes for the defaults module
 */
/* @{ */
typedef enum {
  DEFAULTS_ERROR_OK = 0,            /**< No errors occured. */
  DEFAULTS_ERROR_TYPE_MISMATCH,     /**< String cannot be converted to 
                                         desired type. */
  DEFAULTS_ERROR_NOTFOUND,          /**< Did not find the specified key. */
} DefaultsError;
/* @} */

/** Enumeration result. */
typedef struct _DefaultsEnumResult {
  int numResults;           /**< The number of results. */
  unsigned char **keys;     /**< An array of matching keys. */
  unsigned char **values;   /**< An array of the associated values. */
} DefaultsEnumResult;

int defaults_read(unsigned char *filename);
int defaults_write(unsigned char *filename);
void defaults_free(void);

DefaultsError defaults_get(unsigned char *key, unsigned char **value);
DefaultsError defaults_get_int(unsigned char *key, int *value);
DefaultsError defaults_get_boolean(unsigned char *key, int *value);

void defaults_enum(unsigned char *prefix, DefaultsEnumResult *result);
void defaults_enum_result_free(DefaultsEnumResult *result);

void defaults_add(unsigned char *key, unsigned char *value);
void defaults_update(unsigned char *key, unsigned char *value);

#endif
