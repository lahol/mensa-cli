/*  Libmensa - get information of available meals for a given canteen.
 *  Copyright (c) 2011–8, Holger Langenau
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
/** @defgroup document Read a remote file into memory.
 *  @ingroup libmensa
 */
#ifndef __MENSADOCUMENT_H__
#define __MENSADOCUMENT_H__

#include <stdlib.h>

typedef struct _mensaDocument mensaDocument;

struct _mensaDocument {
    char *data;
    size_t size;
    char *document_url;
};

mensaDocument *mensa_document_get(const char *url);
void mensa_document_free(mensaDocument *doc);

#endif
