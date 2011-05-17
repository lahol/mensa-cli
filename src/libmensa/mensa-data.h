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
 *  Header file with transparent data structures for communication with
 *  an application.
 */
#ifndef __MENSA_DATA_H__
#define __MENSA_DATA_H__

/** @brief A single meal.
 */
/* @{ */
typedef struct _MensaMeal {
  char *description;         /**< @brief Description of the meal. */
  char *type;                /**< @brief The type of the meal. */
  int order;                 /**< @brief The order of the meal.
                              *   Lower values are higher in list. */
} MensaMeal;
/* @} */

/** @brief A group of meals.
 */
/* @{ */
typedef struct _MensaMealGroup {
  MensaMeal *meals;          /**< @brief An array of meals in this group. */
  int meal_count;            /**< @brief The number of meals in this group. */
} MensaMealGroup;
/* @} */

#endif
