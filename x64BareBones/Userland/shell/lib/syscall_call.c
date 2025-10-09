#include "syscall_call.h"

extern long sys_write(int fb, const char *buf);
extern long sys_read (char *buf);

#define STDIN   0
#define STDOUT  1

int print(const char *buf){
    return sys_write(STDOUT, buf);
}

int println(const char *buf){
    return (sys_write(STDOUT, buf) && print("\n"));
}

size_t read(char *buf){
    return sys_read(buf);
}

//Clear
//apagado
//registros del procesador
//hora