#include "./include/syscall_call.h"
#include "./include/commandRead.h"
#include "./tron2/include/map.h"
#include "./tron2/include/types.h"


int main() {
    char buf[256];
    while (1){
        write("- ");
        read(buf);
        cr_dispatch_exact(buf);
    }
    return 0;
}
