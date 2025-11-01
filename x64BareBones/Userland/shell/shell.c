#include "./lib/syscall_call.h"
#include "./lib/commandRead.h"
#include "./tron2/include/map.h"
#include "./tron2/include/types.h"



int main() {
    char buf[256];
    int should_exit = 0;
    while (!should_exit){
        write("- ");
        read(buf);
        should_exit = cr_dispatch_exact(buf);
    }
    return 0;
}
