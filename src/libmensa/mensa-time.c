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
 *  Handling of mensa specific dates.
 */
#define _XOPEN_SOURCE   /* glibc2 needs this */
#include <time.h>
#include "mensa-time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/** @brief Parse time given as string.
 *  The string may be in the form %Y-%m-%d or (case-insensitive) 
 *  today, tomorrow, monday, ..., sunday
 *  @param[in] str The string to parse.
 *  @param[out] date The parsed date.
 *  @return 0 if the date could be parsed, 1 if an error occured or
 *          the string is not in an appropriate form.
 *  @see mensa_translate_date
 */
int mensa_parse_time(char *str, mensaDate *date) {
  time_t current_time;
  time_t target_time;
  struct tm *clt;
  struct tm target;
  char *remainder;

  if (!date) return 1;
  
  time(&current_time);
  clt = localtime(&current_time);
  
  if (str) {
    remainder = strptime(str, "%Y-%m-%d", &target);
  }
  
  if (!str || !remainder || remainder[0] != '\0') {
    target.tm_mday = clt->tm_mday;
    target.tm_mon = clt->tm_mon;
    target.tm_year = clt->tm_year;
    target.tm_hour = clt->tm_hour;
    target.tm_min = clt->tm_min;
    target.tm_sec = clt->tm_sec;
    if (!str || !strcasecmp(str, "today")) {
       ;
    }
    else if (!strcasecmp(str, "tomorrow")) {
      target.tm_mday++;
    }
    else if (!strcasecmp(str, "monday")) {
      target.tm_mday += (7+1-clt->tm_wday)%7;
    }
    else if (!strcasecmp(str, "tuesday")) {
      target.tm_mday += (7+2-clt->tm_wday)%7;
    }
    else if (!strcasecmp(str, "wednesday")) {
      target.tm_mday += (7+3-clt->tm_wday)%7;
    }
    else if (!strcasecmp(str, "thursday")) {
      target.tm_mday += (7+4-clt->tm_wday)%7;
    }
    else if (!strcasecmp(str, "friday")) {
      target.tm_mday += (7+5-clt->tm_wday)%7;
    }
    else if (!strcasecmp(str, "saturday")) {
      target.tm_mday += (7+6-clt->tm_wday)%7;
    }
    else if (!strcasecmp(str, "sunday")) {
      target.tm_mday += clt->tm_wday%7;
    }
    else {
      return 1;
    }
    target_time = mktime(&target);
    memcpy(&target, localtime(&target_time), sizeof(struct tm));
  }
  
  date->day = target.tm_mday;
  date->month = target.tm_mon+1;
  date->year = target.tm_year+1900;
  date->dow = target.tm_wday;
  
  return 0;
}

/** @brief Translate a date.
 *  Uses information of day, month and year to calculate the week and day of the week
 *  from the current time.
 *  @param[in,out] date The date to translate.
 *  @return 0 on success, 1 otherwise.
 */
int mensa_translate_date(mensaDate *date) {
  time_t current_time;
  time_t target_time;
  time(&current_time);
  struct tm *clt = localtime(&current_time);
  struct tm target;
  int wk = 0, dy = 0;
  int daydiff;
  if (!clt) return 1;
  if (!date) return 1;
  
  memset(&target, 0, sizeof(struct tm));
  target.tm_mday = date->day;
  target.tm_mon = date->month-1;
  target.tm_year = date->year-1900;
  
  /* we want full days for difference */
  target.tm_hour = clt->tm_hour;
  target.tm_min = clt->tm_min;
  target.tm_sec = clt->tm_sec;
  target.tm_isdst = clt->tm_isdst;
  
  target_time = mktime(&target);
  
  daydiff = (int)(difftime(target_time, current_time)/86400);  /* seconds / 60 / 60 / 24 -> days*/
  
  wk = ((clt->tm_wday ? clt->tm_wday : 7)+daydiff)/7;
  dy = (clt->tm_wday+daydiff)%7;
  if (dy == 0) wk--;

  date->dow = dy;
  date->week = wk;  
  
  return 0;
}
