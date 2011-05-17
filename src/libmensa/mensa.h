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
/** @defgroup libmensa Mensa menu parsing library.
 *  @file
 *  Main library header file. Includes all other necessary headers.
 */
#ifndef __MENSA_H__
#define __MENSA_H__

#include "mensa-data.h"
#include "mensa-schema.h"
#include "mensa-time.h"

MensaMealGroup* mensa_get_meals(mensaSchema *schema, mensaDate *date);
void mensa_free_meals(MensaMealGroup *meals);

#endif
