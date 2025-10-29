   //#include "../include/syscall.h"
    #include <stdint.h>
    #include "video.h"
    #include "keyboard_handler.h"
    #include "realTimeClock.h"

    extern void enable_interrupts(void);

    typedef struct {
        uint64_t rax;
        uint64_t rbx;
        uint64_t rcx;
        uint64_t rdx;
    } syscall_Registers;

    static int syscall_write(syscall_Registers *regs);
    static int syscall_read(syscall_Registers *regs);
    static void syscall_getDate(syscall_Registers *regs);
    static double syscall_benchmark(syscall_Registers *regs);
    static void syscall_resize(syscall_Registers *regs);

    extern int  vdSetFontScale(int s);
    extern void vdclearScreen(void);


    int syscall_handler(syscall_Registers * regs){
        switch ((int)regs->rax){
        case 0:
            return syscall_read(regs); 
        case 1:
            return syscall_write(regs);
        case 2:
            syscall_clearwindow(regs);
            break;
        case 3:
            syscall_getDate(regs);
            break;
        case 4:
            syscall_resize(regs);
            break;
        case 5:
            return syscall_benchmark(regs);
        default:
            return 0;
        }
    }

    double syscall_benchmark(syscall_Registers *regs){
        uint64_t which = regs->rbx;             // 0=fps, 1=floating, 2=vram
        double res = 0.0;

        switch (which) {
            case 0: res = benchmark_fps();               break;
            case 1: res = benchmark_floating_point();    break;
            case 2: res = benchmark_hardware_access();   break;
            default: res = -1.0;                         break; // inválido
        }

        // Devolver el double como bits en RAX 
        regs->rax = dbl_to_u64_bits(res);

    }


    void syscall_resize(syscall_Registers *regs) {
        int s = str_to_uint_ignore_sign(regs->rbx);     // <-- paso el valor del char * que se pasa desde el assembler a un entero

        // 2) Validar / clamp simple (defensivo en kernel)
        if (s < 1) s = 1;
        if (s > 4) s = 4;         // si esta fuera del rango que printee que esta fuera de rango (FALTA HACER ESTO)

        // 3) Se setea el GD_Scale en el valor que tiene S
        vdSetFontScale(s);

    }


    static void syscall_getDate(syscall_Registers *regs){
        if (regs->rbx == 1){
           vdPrint(getDateString());
           return;
        }
        vdPrint(getTimeString());
        
    }

    static int syscall_write(syscall_Registers *regs){
        if (regs->rbx == 1){
            vdPrint( (const char*) regs->rcx );
        }else {
            vdPrintStyled( (const char*) regs->rcx, 0x00ffffff, 0x00FF0000);
        } 
        return 1;
    }

    void syscall_clearwindow(syscall_Registers *regs){
        return vdclearScreenDB(regs->rbx);
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