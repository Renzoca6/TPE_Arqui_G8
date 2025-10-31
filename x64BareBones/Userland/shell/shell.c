#include "./lib/syscall_call.h"
#include "./lib/commandRead.h"
#include "./tron2/include/map.h"
#include "./tron2/include/types.h"



int main() {
    char buf[256];
    sleep_ms(1);
    write("adada");
    while (1){
        write("- ");
        read(buf);
        cr_dispatch_exact(buf);}
    return 1;
}
