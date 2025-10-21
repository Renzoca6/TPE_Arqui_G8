   //#include "../include/syscall.h"
    #include <stdint.h>
    #include "../include/video.h"

    typedef struct {
        uint64_t rax;
        uint64_t rbx;
        uint64_t rcx;
        uint64_t rdx;
    } syscall_Registers;

    static int syscall_write(syscall_Registers *regs);
    static int syscall_read(syscall_Registers *regs);


    int syscall_handler(syscall_Registers * regs){
        switch ((int)regs->rax){
        case 0:
            return syscall_read(regs); 
        case 1:
            return syscall_write(regs);
        default:
            return 0;
        }
    }

    static int syscall_write(syscall_Registers *regs){
        vdNewline();
        vdPrint("ENTRE A SYSCALL WRITE");
        vdNewline();
        vdPrint("PRINT DE RCX:");
        vdPrint( (const char*) regs->rcx );
        vdNewline();
        char buf[20];
        intToStrSimple(regs->rcx,buf);
        vdPrint("PRINT DE INT TO STRING:");
        vdPrint(buf);
        vdNewline();
        return 1;
    }

    static int syscall_read(syscall_Registers *regs){  
        vdNewline();
        vdPrint("ENTRE A SYSCALL READ");
        vdNewline();
        //hay que codearla pero no nos anda la write
        return 0;
    }