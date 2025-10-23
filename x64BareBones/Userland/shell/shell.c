#include "./lib/syscall_call.h"

int main() {

    char buf[256];
    int i = 0;
    while (1){
        read(buf);
        // funcion que revisa el buffer en buequeda de un comando
        if (i == 4)
        {
            clearwindow(0x00000000);
        }
        i++;
        
    }
    
    return 1;
}
