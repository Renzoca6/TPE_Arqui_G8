#include "./lib/syscall_call.h"
#include "./lib/commandRead.h"

int main() {

    char buf[256];
    while (1){
        read(buf);
        cr_dispatch_exact(buf);}
    
    return 1;
}
