#ifndef REAL_TIME_CLOCK_H
#define REAL_TIME_CLOCK_H

#include <stdint.h>

int getSeconds();
int getMinutes();
int getHours();
int getDay();
int getMonth();
int getYear_YY();
int getYear_YYYY();
void getDateString(char *buffer);
void getTimeString(char *buffer);


#endif