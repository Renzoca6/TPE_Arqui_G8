#include "realTimeClock.h"
#include "io.h"
#include <stdint.h>

// ---------------------------------------------------------------------
// Ajuste de zona horaria (Argentina = UTC-3)
// ---------------------------------------------------------------------
#define TIMEZONE_OFFSET   -3

// ---------------------------------------------------------------------
// Convierte un número BCD (Binary Coded Decimal) a decimal normal
// ---------------------------------------------------------------------
int toNum(uint8_t num) {
    uint8_t high = num >> 4;      // Muevo los 4 bits altos hacia abajo
    uint8_t low  = num & 0x0F;    // Conservo solo los 4 bits bajos
    return (high * 10 + low);     // Ejemplo: 0x54 -> 5*10 + 4 = 54
}

// ---------------------------------------------------------------------
// Lee un registro del RTC (Real Time Clock)
// ---------------------------------------------------------------------
uint8_t timer(uint8_t index) {
    outb(0x70, 0x80 | (index & 0x7F));  // Selecciona el registro
    return inb(0x71);                   // Devuelve el valor leído
}

// ---------------------------------------------------------------------
// Devuelve el año completo (YYYY)
// ---------------------------------------------------------------------
int getYear_YYYY() {
    int yy = toNum(timer(9));       // Año en BCD (ej. 24)
    int cc = getYear_YY();          // Siglo (ej. 20)
    return cc * 100 + yy;           // Combino: 20 * 100 + 24 = 2024
}

// ---------------------------------------------------------------------
// Devuelve los dos primeros dígitos del año (siglo)
// ---------------------------------------------------------------------
int getYear_YY() {
    return toNum(timer(0x32));      // Registro 0x32 contiene el siglo
}

// ---------------------------------------------------------------------
// Devuelve los segundos actuales
// ---------------------------------------------------------------------
int getSeconds() {
    return toNum(timer(0));
}

// ---------------------------------------------------------------------
// Devuelve los minutos actuales
// ---------------------------------------------------------------------
int getMinutes() {
    return toNum(timer(2));
}

// ---------------------------------------------------------------------
// Devuelve la hora actual ajustada a la zona horaria local
// ---------------------------------------------------------------------
int getHours() {
    int h = toNum(timer(4));                // Hora UTC (0–23)
    h = (h + TIMEZONE_OFFSET) % 24;         // Ajusto por zona horaria
    if (h < 0) h += 24;                     // Corrijo si quedó negativa
    return h;
}

// ---------------------------------------------------------------------
// Devuelve el día del mes
// ---------------------------------------------------------------------
int getDay() {
    return toNum(timer(7));
}

// ---------------------------------------------------------------------
// Devuelve el mes actual
// ---------------------------------------------------------------------
int getMonth() {
    return toNum(timer(8));
}

// ---------------------------------------------------------------------
// Devuelve una cadena con la hora actual en formato hh:mm:ss
// Ejemplo: "14:32:05"
// ---------------------------------------------------------------------
char *getTimeString() {
    static char buffer[9];
    int h = getHours();
    int m = getMinutes();
    int s = getSeconds();

    buffer[0] = '0' + (h / 10);
    buffer[1] = '0' + (h % 10);
    buffer[2] = ':';
    buffer[3] = '0' + (m / 10);
    buffer[4] = '0' + (m % 10);
    buffer[5] = ':';
    buffer[6] = '0' + (s / 10);
    buffer[7] = '0' + (s % 10);
    buffer[8] = '\0';

    return buffer;
}

// ---------------------------------------------------------------------
// Devuelve una cadena con la fecha actual en formato dd/mm/yyyy
// Ejemplo: "02/11/2025"
// ---------------------------------------------------------------------
char *getDateString() {
    static char buffer[11];
    int d = getDay();
    int m = getMonth();
    int y = getYear_YYYY();

    buffer[0] = '0' + (d / 10);
    buffer[1] = '0' + (d % 10);
    buffer[2] = '/';
    buffer[3] = '0' + (m / 10);
    buffer[4] = '0' + (m % 10);
    buffer[5] = '/';
    buffer[6] = '0' + (y / 1000) % 10;
    buffer[7] = '0' + (y / 100) % 10;
    buffer[8] = '0' + (y / 10) % 10;
    buffer[9] = '0' + (y % 10);
    buffer[10] = '\0';

    return buffer;
}
