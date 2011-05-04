/** @file 
 *  @ingroup libmensa
 *  Mensa specific date handling.
 */
#ifndef __MENSA_TIME_H__
#define __MENSA_TIME_H__

/** Necessary information of a date. */
/* @{ */
typedef struct _mensaDate {
  int day;               /**< Day of month (1-31) */
  int month;             /**< Month of year (1-12) */
  int year;              /**< Year (like %Y) */
  int dow;               /**< Day of week (0-6, 0 being sunday) */
  int week;              /**< Week number relative to current week, 0 being current week. */ 
} mensaDate;
/* @} */

int mensa_parse_time(char *str, mensaDate *date);
int mensa_translate_date(mensaDate *date);

#endif
