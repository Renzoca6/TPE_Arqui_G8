#include "./lib/syscall_call.h"
#include "./lib/commandRead.h"
#include "./utils/tronUtils.h"


int main() {
    char buf[256];
    draw_grid(10, 100, 100, 921, 701);
    while (1){
        write("-");
        read(buf);
        cr_dispatch_exact(buf);}
    
    return 1;
}
