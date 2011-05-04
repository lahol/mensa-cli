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
