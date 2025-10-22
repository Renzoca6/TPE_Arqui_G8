   //#include "../include/syscall.h"
    #include <stdint.h>
    #include "video.h"
    #include "keyboard_handler.h"

    extern void enable_interrupts(void);

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
        if (regs->rbx == 1){
            vdPrint( (const char*) regs->rcx );
        }else {
            vdPrintCharStyled( (const char*) regs->rcx, 0x00ffffff, 0x00FF0000);
        } 
        
        
        
        return 1;
    }

static int syscall_read(syscall_Registers *regs) {
    char   *buf = (char *)regs->rbx;
    int  size = 0;

    clearKeyBoardBuffer();                 //limpio el buffer del teclado

    enable_interrupts();        //habilito (Interrupt Flag)

    while (1){
        if (hasNextKey()) {
            KeyBufferStruct k = getNextKey();
            if (k.is_pressed){
                if (k.key == '\n') {
                    vdPrintChar('\n');         // enter
                    buf[size] = '\0';          // pongo null
                    return (int)size;          
                } else if (k.key == '\b') {
                    if (size > 0) {
                        size--;
                        buf[size] = '\0';
                        vdBackSpace();           // borro
                    }
                } else if (k.key) {
                    if (size + 1 < 256) {      // deja espacio para \0
                        buf[size++] = k.key;
                        vdPrintChar(k.key);    //print
                    }else{
                        vdPrintChar('\n');         // enter
                        buf[size] = '\0';          // pongo null
                        return (int)size;  
                    }
                }
            }
        } 
    }

    //falta funcion para apagar las interrupts
}