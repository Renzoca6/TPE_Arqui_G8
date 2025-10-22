#include "./lib/syscall_call.h"

int main() {
    char buf[256];
    while (1){
        read(buf);
        // funcion que revisa el buffer en buequeda de un comando
        println(buf);
    }
    
    

    return 1;
}
