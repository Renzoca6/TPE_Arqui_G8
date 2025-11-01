#ifndef REAL_TIME_CLOCK_H
#define REAL_TIME_CLOCK_H

#include <stdint.h>

/* ===================== */
/*    Reloj en tiempo real */
/* ===================== */

int getSeconds(void);
int getMinutes(void);
int getHours(void);
int getDay(void);
int getMonth(void);
int getYear_YY(void);
int getYear_YYYY(void);

/**
 * Devuelve una cadena estática con la fecha formateada.
 */
char *getDateString(void);

/**
 * Devuelve una cadena estática con la hora formateada.
 */
char *getTimeString(void);

#endif /* REAL_TIME_CLOCK_H */
