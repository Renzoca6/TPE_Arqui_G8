#include "syscall_call.h"

extern int sys_write(int fb, const char* buf);
extern int sys_read (char *buf);

#define STDIN   0
#define STDOUT  1

int write(const char* buf){
    return sys_write(STDOUT, buf);
}

int println(const char* buf){
    return (sys_write(STDOUT, buf) && write("\n"));
}

int read (char *buf){
    return sys_read(buf);
}

//Clear
//apagado
//registros del procesador
//hora