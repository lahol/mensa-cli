#ifndef __MENSA_TIME_H__
#define __MENSA_TIME_H__

typedef struct _mensaDate {
  int day;
  int month;
  int year;
  int dow;
  int week;
} mensaDate;

int mensa_parse_time(char *str, mensaDate *date);
int mensa_translate_date(mensaDate *date);

#endif