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
