#include "realTimeClock.h"
#include "io.h"
#include <stdint.h>  

#define TIMEZONE_OFFSET -3

int toNum(uint8_t num){
    uint8_t high = num >> 4;    //muevo todo para la derecha entoces la alta queda abajo
    uint8_t low = num & 0X0F;   // hago and solo de la parte baja y por ende borra la alta 
    return (high *10 + low);    // estoy haciendo el high x10  + low ejmplo 5*10 + 4 = 54
}

uint8_t timer(uint8_t index) {
    outb(0x70, 0x80 | (index & 0x7F));
    return inb(0x71);
}


int getYear_YYYY() {
    int yy = toNum(timer(9));      
    int cc = getYear_YY();         
    return cc * 100 + yy;
}

int getYear_YY(){
    return toNum(timer(0x32));
}


int getSeconds(){
    return toNum(timer(0));
}
   

int getMinutes(){
    return toNum(timer(2));
}


// el timer devuelve la hora en utc-0 entoces la tengo que cambiar 
int getHours(){
    int h = toNum(timer(4));
    h = (h + TIMEZONE_OFFSET) % 24;
    if (h < 0) h += 24;
    return h;
}

int getDay(){
    return toNum(timer(7));
}

int getMonth(){
    return toNum(timer(8));
}




// hh/mm/ss\0
void getTimeString(char *buffer) {
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
}
// dd/mm/yyyy\0
void getDateString(char *buffer) {
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
}