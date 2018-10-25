/*  Libmensa - get information of available meals for a given canteen.
 *  Copyright (c) 2011, Holger Langenau
 *
 *  This file is part of Libmensa.
 *
 *  Libmensa is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  Libmensa is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Libmensa.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Adapted from https://curl.haxx.se/libcurl/c/getinmemory.html */
#include "mensa-document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <curl/curl.h>

static size_t _mensa_document_write_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    mensaDocument *doc = (mensaDocument *)userp;

    char *ptr = realloc(doc->data, doc->size + realsize + 1);
    if (ptr == NULL) {
        return 0;
    }

    doc->data = ptr;
    memcpy(&(doc->data[doc->size]), contents, realsize);
    doc->size += realsize;
    doc->data[doc->size] = 0;

    return realsize;
}

mensaDocument *mensa_document_get(const char *url)
{
    if (!url)
        return NULL;

    mensaDocument *doc = malloc(sizeof(mensaDocument));
    assert(doc);
    memset(doc, 0, sizeof(mensaDocument));

    doc->data = malloc(1);
    doc->size = 0;

    doc->document_url = strdup(url);

    CURL *curl_handle;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);

    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, _mensa_document_write_cb);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)doc);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        goto error;
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return doc;

error:
    mensa_document_free(doc);
    return NULL;
}

void mensa_document_free(mensaDocument *doc)
{
    if (doc) {
        free(doc->data);
        free(doc->document_url);
        free(doc);
    }
}
