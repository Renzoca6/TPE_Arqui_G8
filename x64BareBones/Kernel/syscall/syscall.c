//#include "../include/syscall.h"
#include <stdint.h>
#include "video.h"
#include "keyboard_handler.h"
#include "realTimeClock.h"
#include "benchmark.h"

extern void enable_interrupts(void);

typedef struct {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rdi;

} syscall_Registers;

static void syscall_write(syscall_Registers *regs);
static void  syscall_read(syscall_Registers *regs);
static void syscall_getDate(syscall_Registers *regs);
static void syscall_benchmark(syscall_Registers *regs);
static void syscall_resize(syscall_Registers *regs);
static void syscall_clearwindow(syscall_Registers *regs);
static void syscall_write_at_VRAM(syscall_Registers * regs);
static void syscall_write_at_BACK(syscall_Registers * regs);     
static void  syscall_present_fullframe();      


int syscall_handler(syscall_Registers *regs) {
    switch ((int)regs->rax) {
        case 0:
            syscall_read(regs);
            break;
        case 1:
            syscall_write(regs);
            break;
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
            // benchmark: deja el resultado en regs->rax
            syscall_benchmark(regs);
            break;
        case 6:
            syscall_write_at_VRAM(regs);    
            break; 
        case 7:
            syscall_write_at_BACK(regs);    
            break; 
        case 8:
            syscall_present_fullframe();    
            break; 
        default:
            return 0;
    }
    return 0;
}


static void syscall_write_at_VRAM(syscall_Registers * regs){
    return;
}
static void syscall_write_at(syscall_Registers * regs){
    return;
}
static void syscall_write_at_BACK(syscall_Registers * regs){
    return;
}



static void  syscall_present_fullframe(){
    present_fullframe();
}  

static void syscall_write(syscall_Registers *regs) {
    if (regs->rbx == 1) {
        vdPrint( (const char*) regs->rcx, PIXEL_VRAM);
    } else {
        vdPrintStyled( (const char*) regs->rcx, 0x00ffffff, 0x00FF0000, PIXEL_VRAM);
    }
}

/*
 * syscall_benchmark:
 * - entra con regs->rbx = cuál benchmark correr
 * - sale con regs->rax = resultado
 */
static void syscall_benchmark(syscall_Registers *regs) {
    uint64_t which = regs->rbx;             // 0=fps, 1=floating, 2=vram
    uint64_t res = 0;

    switch (which) {
        case 0: res = benchmark_fps();             break;
        case 1: res = benchmark_floating_point();  break;
        case 2: res = benchmark_hardware_access(); break;
        default: res = (uint64_t)-1;               break; // inválido
    }

    // devolver el entero directamente por RAX
    regs->rax = res;
    // no hace falta return res, porque el kernel ya tiene regs
}


static void syscall_resize(syscall_Registers *regs) {
    int s = str_to_uint_ignore_sign((char *)regs->rbx);     // <-- paso el valor del char * que se pasa desde el assembler a un entero

    // 2) Validar / clamp simple (defensivo en kernel)
    if (s < 1) s = 1;
    if (s > 4) s = 4;         // si esta fuera del rango que printee que esta fuera de rango (FALTA HACER ESTO)

    // 3) Se setea el GD_Scale en el valor que tiene S
    vdSetFontScale(s);
}


static void syscall_getDate(syscall_Registers *regs) {
    if (regs->rbx == 1) {
       vdPrint(getDateString(), PIXEL_VRAM);
       return;
    }
    vdPrint(getTimeString(), PIXEL_VRAM);
}

static void syscall_clearwindow(syscall_Registers *regs) {
    vdclearScreenDB(regs->rbx);
}


static void syscall_read(syscall_Registers *regs) {
    char *buf = (char *)regs->rbx;
    int   size = 0;

    clearKeyBoardBuffer();                 //limpio el buffer del teclado

    enable_interrupts();        //habilito (Interrupt Flag)

    while (1) {
        if (hasNextKey()) {
            KeyBufferStruct k = getNextKey();
            if (k.is_pressed) {
                if (k.key == '\n') {
                    vdPrintChar('\n', PIXEL_VRAM);         // enter
                    buf[size] = '\0';          // pongo null
                    regs->rax = (uint64_t)size; // también lo dejo en rax por si el user lo espera ahí
                    return;
                } else if (k.key == '\b') {
                    if (size > 0) {
                        size--;
                        buf[size] = '\0';
                        vdBackSpace(PIXEL_VRAM);           // borro
                    }
                } else if (k.key) {
                    if (size + 1 < 256) {      // deja espacio para \0
                        buf[size++] = k.key;
                        vdPrintChar(k.key, PIXEL_VRAM);    //print
                    } else {
                        vdPrintChar('\n', PIXEL_VRAM);         // enter
                        buf[size] = '\0';          // pongo null
                        regs->rax = (uint64_t)size;
                        return;
                    }
                }
            }
        }
    }

    //falta funcion para apagar las interrupts
}
