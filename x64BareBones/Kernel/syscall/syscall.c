    #include "../include/syscall.h"
    #include <stdint.h>
    #include "../include/video.h"



    int syscall_write(syscall_Registers *regs);
    void syscall_read(syscall_Registers *regs);


    void syscall_handler(syscall_Registers * regs){
        switch ((int)regs->rax)
        {
        case 0:
        syscall_read(regs); 
        break;

        case 1:
            syscall_write(regs);
            break;
        default:
            break;
        }
    }

    int syscall_write(syscall_Registers *regs){
        char * str = (char*)regs->rbx;  // me guardo el puntero al inicio del string 
        vdPrint(str);                   // printeo el string
        return 1;
    }

    void syscall_read(syscall_Registers *regs){

    }