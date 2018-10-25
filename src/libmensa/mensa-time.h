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
/** @file
 *  @ingroup libmensa
 *  Mensa specific date handling.
 */
#ifndef __MENSA_TIME_H__
#define __MENSA_TIME_H__

/** @brief Necessary information of a date. */
/* @{ */
typedef struct _mensaDate {
    int day;               /**< @brief Day of month (1-31) */
    int month;             /**< @brief Month of year (1-12) */
    int year;              /**< @brief Year (like %Y) */
    int dow;               /**< @brief Day of week (0-6, 0 being sunday) */
    int week;              /**< @brief Week number relative to current week, 0 being current week. */
} mensaDate;
/* @} */

int mensa_parse_time(char *str, mensaDate *date);
int mensa_translate_date(mensaDate *date);

#endif
