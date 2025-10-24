#include "./lib/syscall_call.h"

int main() {

    char buf[256];
    int i = 0;
    while (1){
        read(buf);
        cr_dispatch_exact(buf);
        println(buf);
    }
    
    return 1;
}
