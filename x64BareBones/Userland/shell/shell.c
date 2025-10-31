#include "./lib/syscall_call.h"
#include "./lib/commandRead.h"
#include "./utils/tronUtils.h"
#include "./tron/tron_main.h"


int main() {
    char buf[256];
    start();
    while (1){
        write("- ");
        read(buf);
        cr_dispatch_exact(buf);}
    return 1;
}
