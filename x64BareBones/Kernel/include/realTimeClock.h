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
char * getDateString();
char * getTimeString();


#endif