#include "syscall_call.h"

extern int sys_write(int fb, const char* buf);
extern int sys_read (char *buf);
extern int sys_clearwindow(uint32_t* color);

#define STDIN   0
#define STDOUT  1

int write(const char* buf){
    return sys_write(STDOUT, buf);
}

int println(const char* buf){
    return (sys_write(STDOUT, buf) && write("\n"));
}

int printError(const char* buf){
    return (sys_write(STDIN, buf) && write("\n"));
}

int read (char *buf){
    return sys_read(buf);
}

int clearwindow(uint32_t* color){
    sys_clearwindow(color);
}


//Clear
//apagado
//registros del procesador
//hora
//fps
//ETC