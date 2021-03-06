/*  Mensa-CLI - a command line interface for libmensa
 *  Copyright (C) 2011, Holger Langenau
 *
 *  This file is part of Mensa-CLI.
 *
 *  Mensa-CLI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Mensa-CLI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Mensa-CLI.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file
 *  @ingroup defaults
 *  Implementation of resource file and defaults handling.
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "defaults.h"
#include "utils.h"
#include <ctype.h>

/** @name Default list flags.
 *  @internal
 */
/** @{ */
#define DEFAULTS_LIST_FLAG_NONE          0     /**< @brief no flags @hideinitializer */
#define DEFAULTS_LIST_FLAG_MODIFIED      1<<0  /**< @brief was modified by user (command) @hideinitializer */
#define DEFAULTS_LIST_FLAG_WRITTEN       1<<1  /**< @brief has been written to rc file @hideinitializer */
/** @} */

/** @name Line types of rc file
 *  @internal
 */
/** @{ */
#define _DEFAULTS_RC_LINE_KEYVALUE       1     /**< @brief Line is a key/value pair. @hideinitializer */
#define _DEFAULTS_RC_LINE_EMPTY          2     /**< @brief An empty line, possibly whitespaces. @hideinitializer */
#define _DEFAULTS_RC_LINE_COMMENT        3     /**< @brief Line contains a comment. @hideinitializer */
/** @} */

/** @brief Defaults list element.
 *  @internal
 */
/* @{ */
typedef struct _DefaultsList DefaultsList;
struct _DefaultsList {
    char *key;                                   /**< @brief Name of the key. */
    char *value;                                 /**< @brief The associated value. */
    unsigned long flags;                         /**< @brief Flags of this pair. */
    DefaultsList *next;                          /**< @brief Next element in list. */
};
/* @} */

/** List of defaults.
 *  @internal
 */
DefaultsList *_defaults_list = NULL;

DefaultsList *_defaults_set(char *key, char *value);
DefaultsList *_defaults_get_key(char *key);
int _defaults_rc_read_line(FILE *stream,
                           char **key,
                           char **val,
                           int *type, long *vstart, long *vend);
void _defaults_rc_skip_spaces(FILE *stream);
void _defaults_rc_goto_eol(FILE *stream);
void _defaults_rc_copy_line(FILE *src, FILE *dst);

/** Read default settings from an rc-file.
 *  @param[in] filename The file to read from.
 *  @return 0 if the file could be read or 1 if an error occured.
 */
int defaults_read(char *filename)
{
    FILE *f;
    int type;
    char *key;
    char *val;
    if (!filename) {
        return 1;
    }

    if ((f = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Could not read rc file \"%s\": ", filename);
        perror("");
        return 1;
    }

    while (!feof(f)) {
        if (_defaults_rc_read_line(f, &key, &val, &type, NULL, NULL) != 0) {
            fprintf(stderr, "Error parsing rc file.\n");
            fclose(f);
            return 1;
        }
        else {
            if (type == _DEFAULTS_RC_LINE_KEYVALUE) {
                /* insert to defaults */
                if (key && val) {
                    defaults_add(key, val);
                }
                if (key) {
                    free(key);
                    key = NULL;
                }
                if (val) {
                    free(val);
                    val = NULL;
                }
            }
            _defaults_rc_goto_eol(f);
        }
    }

    fclose(f);
    return 0;
}

/** Skip spaces and tabs from the current position of the stream.
 *  @param[in] stream A pointer to a file structure, open to read.
 *  @internal
 */
void _defaults_rc_skip_spaces(FILE *stream)
{
    int tok;
    while (1) {
        tok = fgetc(stream);
        if (feof(stream)) {
            return;
        }

        if (tok != ' ' && tok != '\t') {
            fseek(stream, -1L, SEEK_CUR); /* ungetc(tok, stream);*/
            return;
        }
    }
}

/** Read characters until the end of line/file is reached.
 *  @param[in] stream A pointer to a file structure, open to read.
 *  @internal
 */
void _defaults_rc_goto_eol(FILE *stream)
{
    int tok;
    do {
        tok = fgetc(stream);
    }
    while (!feof(stream) && tok != '\n');
}

/** Copy the (rest of the) line from src to dst.
 *  @param[in] src A pointer to a file structure, open to read.
 *  @param[in] dst A pointer to a file structure, open to write.
 *  @internal
 */
void _defaults_rc_copy_line(FILE *src, FILE *dst)
{
    int tok;
    if (!src || !dst) return;
    while (1) {
        tok = fgetc(src);
        if (feof(src)) {
            return;
        }
        else {
            fputc(tok, dst);
        }
        if (tok == '\n') {
            return;
        }
    }
}

/** Reads a single line of the rc file and returns a key/value pair
 *  if appropriate. If a line contains only whitespaces type is set
 *  to _DEFAULTS_RC_LINE_EMPTY, if it starts (after possible whitespaces)
 *  with a pound (#) it is a comment and thus type is set to
 *  _DEFAULTS_RC_LINE_COMMENT.
 *  If a correct key/value pair is found type is set to
 *  _DEFAULTS_RC_LINE_KEYVALUE and key and val are set respectively.
 *  Furthermore vstart and vend are set to the first token of the value
 *  or the first token after the value.
 *  If all went well the file pointer is set to the beginning of the line.
 *  Use _defaults_rc_goto_eol() to eat the line.
 *  @param stream The file to read from
 *  @param key    Pointer to a string to hold the key. Memory will be
 *                allocated by the procedure. The caller should free
 *                this with free().
 *  @param val    Pointer to a string to hold the value. Memory will be
 *                allocated by the procedure. The caller should free
 *                this with free().
 *  @param type   The type of the line.
 *  @param vstart Offset of the first character of the value.
 *  @param vend   Offset of the first character after the value.
 *  @return       Non-zero if an error occured.
 *  @internal
 */
int _defaults_rc_read_line(FILE *stream,
                           char **key,
                           char **val,
                           int *type, long *vstart, long *vend)
{
    int tok;
    int key_len;
    int val_len;
    int i;
    long key_start;
    long val_start;
    long last_non_space;
    long start_line;

    if (key) *key = NULL;
    if (val) *val = NULL;

    start_line = ftell(stream);

    _defaults_rc_skip_spaces(stream);
    key_start = ftell(stream);

    tok = fgetc(stream);  /* if we reach eof with seek cur, eof-flag is not set yet *
                         * so we try to read the token here                       */

    if (feof(stream)) {
        if (type) *type = _DEFAULTS_RC_LINE_EMPTY;
        fseek(stream, start_line, SEEK_SET);
        return 0;
    }
    if (tok == '#') { /* this is a comment */
        if (type) *type = _DEFAULTS_RC_LINE_COMMENT;
        fseek(stream, start_line, SEEK_SET);
        return 0;
    }
    else if (tok == '\n') {  /* already reached end of line */
        if (type) *type = _DEFAULTS_RC_LINE_EMPTY;
        fseek(stream, start_line, SEEK_SET);
        return 0;
    }
    else {
        key_len = 0;
        do {
            key_len++;
            tok = fgetc(stream);
            /* the file should not end here and there should be no newline*/
            if (feof(stream) || tok == '\n') {
                return 1;
            }
        }
        while (tok != ' ' && tok != '=' && tok != '\t');

        /* if we haven't read the = yet, skip spaces and try to read it*/
        if (tok != '=') {
            _defaults_rc_skip_spaces(stream);
            tok = fgetc(stream);
            /* next token no = so return an error */
            if (tok != '=') {
                return 1;
            }
        }
        _defaults_rc_skip_spaces(stream);

        val_start = ftell(stream);
        val_len = 0;
        last_non_space = 0;
        while (!feof(stream)) {
            tok = fgetc(stream);
            if (tok != '\n' && !feof(stream)) {
                val_len++;
                if (tok != ' ' && tok != '\t') {
                    last_non_space = val_len;
                }
            }
            else {
                break;
            }
        }

        /* cut of trailing spaces */
        val_len = last_non_space;

        if (key) {
            *key = malloc(sizeof(char *)*(key_len+1));
            assert(*key);
            /* goto beginning of key */
            fseek(stream, key_start, SEEK_SET);
            for (i = 0; i < key_len; i++) {
                (*key)[i] = (char)fgetc(stream);
            }
            (*key)[key_len] = '\0';
        }

        if (val) {
            *val = malloc(sizeof(char *)*(val_len+1));
            assert(*val);
            /* goto beginning of value */
            fseek(stream, val_start, SEEK_SET);
            for (i = 0; i < val_len; i++) {
                (*val)[i] = (char)fgetc(stream);
            }
            (*val)[val_len] = '\0';
        }

        if (vstart) *vstart = val_start;
        if (vend) *vend = val_start+val_len;
        if (type) *type = _DEFAULTS_RC_LINE_KEYVALUE;

        fseek(stream, start_line, SEEK_SET);
        return 0;
    }
}

/** Write the current defaults to the specified file. Only defaults
 *  that were set with defaults_update() are written. If the file
 *  exists the content is left untouched and only the value of
 *  updated defaults is replaced.
 *  @see defaults_update()
 *  @param[in] filename The name of the file to write to.
 *  @return 0 on success, 1 otherwise.
 */
int defaults_write(char *filename)
{
    DefaultsList *setting = NULL;
    FILE *cfgfile = NULL;
    FILE *tmp = NULL;
    int file_exists = 0;
    long vstart, vend;
    long start, i;
    char *key;
    int ltype;
    int first;
    int eof = EOF;
    int res;
    int tok;

    /* no filename specified */
    if (!filename) {
        return 1;
    }

    /* if file exists read contents from file to tmpfile */
    /* file could not be read */
    if ((cfgfile = fopen(filename, "r")) == NULL) {
        file_exists = 0;
    }
    else {
        file_exists = 1;
        tmp = tmpfile();
        if (!tmp) {
            perror("Could not create temporary file");
            fclose(cfgfile);
            return 1;
        }
        while (!feof(cfgfile)) {
            tok = fgetc(cfgfile);
            if (tok != EOF) {
                fputc(tok, tmp);
            }
        }

        rewind(tmp);
        fclose(cfgfile);
    }
    /* check file */
    if (file_exists && tmp) {
        while (!feof(tmp)) {
            if (_defaults_rc_read_line(tmp, NULL, NULL, NULL, NULL, NULL) != 0) {
                fprintf(stderr, "You have a syntax error in your rc file `%s'.\n",
                        filename);
                fclose(tmp);
                return 1;
            }
            else {
                _defaults_rc_goto_eol(tmp);
            }
        }
    }

    /* open file */
    if ((cfgfile = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "Could not open `%s' for writing: ", filename);
        perror("");
        fclose(tmp);
        return 1;
    }

    /* read tmpfile line by line and check for setting */
    if (file_exists && tmp) {
        rewind(tmp);
        while (!feof(tmp)) {
            if (_defaults_rc_read_line(tmp, &key, NULL, &ltype, &vstart, &vend) != 0) {
                fprintf(stderr, "Unexpected error reading temporary file.\n");
                fclose(tmp);
                fclose(cfgfile);
                return 1;
            }
            if (ltype == _DEFAULTS_RC_LINE_KEYVALUE) {
                if (key) {
                    setting = _defaults_get_key(key);
                    if (setting) {  /* setting exists*/
                        if (setting->flags & DEFAULTS_LIST_FLAG_MODIFIED) {
                            start = ftell(tmp);
                            /* copy line till value */
                            for (i = start; i < vstart; i++) {
                                fputc(fgetc(tmp), cfgfile);
                            }

                            /* write value */
                            if (setting->value && setting->value[0] != '\0') {
                                fprintf(cfgfile, "%s", setting->value);
                            }

                            /* jump to character after value */
                            fseek(tmp, vend, SEEK_SET);

                            /* write rest of line*/
                            _defaults_rc_copy_line(tmp, cfgfile);

                            /* set flag to written */
                            setting->flags |= DEFAULTS_LIST_FLAG_WRITTEN;
                        }
                        else { /* if not modified write this as it is */
                            _defaults_rc_copy_line(tmp, cfgfile);
                        }
                    }
                }
            }
            else {
                _defaults_rc_copy_line(tmp, cfgfile);
            }
        }
        fclose(tmp);
    }

    /* write all modified settings that have not been written to cfgfile */
    setting = _defaults_list;
    first = 1;
    while (setting) {
        if ((setting->flags & DEFAULTS_LIST_FLAG_MODIFIED) &&
                !(setting->flags & DEFAULTS_LIST_FLAG_WRITTEN)) {
            if (first) {
                fputc('\n', cfgfile);
                first = 0;
            }
            if (setting->key) {
                fprintf(cfgfile, "%s = ", setting->key);
                if (setting->value && setting->value[0] != '\0') {
                    fprintf(cfgfile, "%s", setting->value);
                }
                fputc('\n', cfgfile);
            }
            setting->flags |= DEFAULTS_LIST_FLAG_WRITTEN;
        }
        setting = setting->next;
    }

    fclose(cfgfile);
    return 0;
}

/** Free all allocated memory for the defaults module.
 */
void defaults_free(void)
{
    DefaultsList *tmp;
    while (_defaults_list) {
        tmp = _defaults_list->next;
        if (_defaults_list->key) {
            free(_defaults_list->key);
        }
        if (_defaults_list->value) {
            free(_defaults_list->value);
        }
        free(_defaults_list);
        _defaults_list = tmp;
    }
}

/** Get the string representation of a value assigned to a given key.
 *  @param[in] key The key to get the value for.
 *  @param[out] value Pointer to a pointer to hold the value if found.
 *                    Should be freed by the caller using free().
 *  @return An error code.
 */
DefaultsError defaults_get(char *key, char **value)
{
    DefaultsList *tmp = _defaults_get_key(key);
    if (value) {
        if (!tmp) {
            *value = NULL;
            return DEFAULTS_ERROR_NOTFOUND;
        }
        else {
            if (tmp->value) {
                *value = strdup(tmp->value);
                assert(*value);
                return DEFAULTS_ERROR_OK;
            }
            else {
                *value = NULL;
                return DEFAULTS_ERROR_OK;
            }
        }
    }
    if (!tmp) {
        return DEFAULTS_ERROR_NOTFOUND;
    }
    else {
        return DEFAULTS_ERROR_OK;
    }
}

/** Get the integer value to a given key.
 *  @param[in] key The key to get the value for.
 *  @param[out] value A pointer to a variable to write the value to.
 *  @return An error code.
 */
DefaultsError defaults_get_int(char *key, int *value)
{
    DefaultsList *tmp = _defaults_get_key(key);
    int i;
    if (!tmp) {
        return DEFAULTS_ERROR_NOTFOUND;
    }
    if (tmp->value) {
        for (i = 0; tmp->value[i] != '\0'; i++) {
            if (!isdigit(tmp->value[i])) {
                return DEFAULTS_ERROR_TYPE_MISMATCH;
            }
        }
        if (value) *value = atoi(tmp->value);
        return DEFAULTS_ERROR_OK;
    }
    else {
        if (value) *value = 0;
        return DEFAULTS_ERROR_OK;
    }
}

/** Get the boolean value to a given key. The strings yes, y, 1, + are
 *  considered to be true, no, n, 0, - to be false.
 *  @param[in] key The key to get the value for.
 *  @param[out] value A pointer to a variable to write the value to.
 *  @return An error code.
 */
DefaultsError defaults_get_boolean(char *key, int *value)
{
    DefaultsList *tmp = _defaults_get_key(key);
    int v;
    if (!tmp) {
        return DEFAULTS_ERROR_NOTFOUND;
    }
    if (tmp->value) {
        if (!strcasecmp(tmp->value, "yes") ||
                !strcasecmp(tmp->value, "y") ||
                !strcmp(tmp->value, "1") ||
                !strcmp(tmp->value, "+") ||
                !strcasecmp(tmp->value, "on") ||
                !strcasecmp(tmp->value, "true") ||
                !strcasecmp(tmp->value, "t") ||
                !strcasecmp(tmp->value, "enabled")) {
            v = 1;
        }
        else if (!strcasecmp(tmp->value, "no") ||
                 !strcasecmp(tmp->value, "n") ||
                 !strcmp(tmp->value, "0") ||
                 !strcmp(tmp->value, "-") ||
                 !strcasecmp(tmp->value, "off") ||
                 !strcasecmp(tmp->value, "false") ||
                 !strcasecmp(tmp->value, "f") ||
                 !strcasecmp(tmp->value, "disabled")) {
            v = 0;
        }
        else {
            return DEFAULTS_ERROR_TYPE_MISMATCH;
        }
        if (value) *value = v;
        return DEFAULTS_ERROR_OK;
    }
    else {
        if (value) *value = 0;
        return DEFAULTS_ERROR_OK;
    }
}

/** Enumerate default settings with the specified prefix and store them
 *  in result. If prefix is NULL return all.
 *  @param[in] prefix Prefix of the setting or NULL
 *  @param[out] result Pointer to the result struct. Should be freed with
 *                     defaults_enum_result_free().
 */
void defaults_enum(char *prefix, DefaultsEnumResult *result)
{
    UtilsList *res_list = NULL;
    UtilsList *res_next = NULL;
    DefaultsList *tmp = NULL;
    int prefix_len;
    int i;
    if (!result) {
        return;
    }

    result->numResults = 0;
    result->keys = NULL;
    result->values = NULL;

    if (prefix && (prefix_len = strlen(prefix)) > 0) {
        tmp = _defaults_list;
        while (tmp) {
            if (tmp->key) {
                if (strncmp(prefix, tmp->key, prefix_len) == 0) {
                    res_list = utils_list_prepend(res_list, (void *)tmp);
                    result->numResults++;
                }
            }
            tmp = tmp->next;
        }
    }
    else { /* no prefix -> get all settings*/
        tmp = _defaults_list;
        while (tmp) {
            res_list = utils_list_prepend(res_list, (void *)tmp);
            result->numResults++;
            tmp = tmp->next;
        }
    }

    result->keys = malloc(sizeof(char *)*result->numResults);
    assert(result->keys);
    memset(result->keys, 0, sizeof(char *)*result->numResults);

    result->values = malloc(sizeof(char *)*result->numResults);
    assert(result->values);
    memset(result->values, 0, sizeof(char *)*result->numResults);

    /*  for (i = result->numResults-1; i >= 0 && res_list; i--, res_list = res_next) {*/
    /* defaults list is also in reverse order */
    for (i = 0; i < result->numResults && res_list; i++, res_list = res_next) {
        res_next = res_list->next;
        tmp = ((DefaultsList *)res_list->data);
        free(res_list);
        if (tmp) {
            if (tmp->key) {
                result->keys[i] = strdup(tmp->key);
                assert(result->keys[i]);
            }
            if (tmp->value) {
                result->values[i] = strdup(tmp->value);
                assert(result->values[i]);
            }
        }
    }
}

/** Free all allocated memory for the result of defaults_enum().
 *  @param[in] result A pointer to the result object to be freed.
 */
void defaults_enum_result_free(DefaultsEnumResult *result)
{
    int i;
    if (result) {
        if (result->keys) {
            for (i = 0; i < result->numResults; i++) {
                if (result->keys[i]) {
                    free(result->keys[i]);
                }
            }
            free(result->keys);
        }
        if (result->values) {
            for (i = 0; i < result->numResults; i++) {
                if (result->values[i]) {
                    free(result->values[i]);
                }
            }
            free(result->values);
        }
        result->numResults = 0;
        result->keys = NULL;
        result->values = NULL;
    }
}

/** Set a key/value pair. If the key exists the value is overwritten.
 *  If it doesn't a new set is inserted.
 *  @param[in] key The name of the key to set the value for.
 *  @param[in] value The string representation of the value or NULL.
 *  @return A pointer to the set modified or inserted.
 *  @internal
 */
DefaultsList *_defaults_set(char *key, char *value)
{
    DefaultsList *tmp;
    if (!key) return NULL;
    tmp = _defaults_get_key(key);
    if (!tmp) {
        tmp = malloc(sizeof(DefaultsList));
        assert(tmp);

        memset(tmp, 0, sizeof(DefaultsList));
        tmp->next = _defaults_list;
        tmp->key = strdup(key);
        assert(tmp->key);

        _defaults_list = tmp;
    }
    if (value) {
        tmp->value = strdup(value);
        assert(tmp->value);
    }
    else {
        tmp->value = NULL;
    }

    return tmp;
}

/** Set a key/value pair. If the key exists the value is overwritten.
 *  If it doesn't a new set is inserted.
 *  This setting only holds for the time the program is running. If
 *  you want to write it to the rc-file with defaults_write()
 *  use defaults_update() instead.
 *  @param[in] key The name of the key to set the value for.
 *  @param[in] value The string representation of the value or NULL.
 *  @see defaults_update()
 */
void defaults_add(char *key, char *value)
{
    _defaults_set(key, value);
}

/** Set a key/value pair. If the key exists the value is overwritten.
 *  If it doesn't a new set is inserted. Also mark the setting as
 *  modified so that a subsequent call to defaults_write() stores this
 *  in the rc-file.
 *  @param[in] key The name of the key to set the value for.
 *  @param[in] value The string representation of the value or NULL.
 */
void defaults_update(char *key, char *value)
{
    DefaultsList *tmp = _defaults_set(key, value);
    if (tmp) {
        tmp->flags |= DEFAULTS_LIST_FLAG_MODIFIED;
    }
}

/** Find a given key in the defaults list.
 *  @param[in] key The name of the key.
 *  @return Pointer to the existing one or NULL if the key was not found.
 *  @internal
 */
DefaultsList *_defaults_get_key(char *key)
{
    DefaultsList *tmp = _defaults_list;
    if (!key) return NULL;
    while (tmp) {
        if (!strcmp(tmp->key, key)) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}
