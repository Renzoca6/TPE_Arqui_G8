#include "syscall_call.h"
#include "../utils/utils.h"

void print_benchmark(void) {
    // 1) limpiar pantalla (negro)
    clearwindow(0x00000000);

    // 2) agrandar letra (podés probar "3" o "4")
    int size = 16;
    do_resize("2");

    // 3) obtener tamaño de pantalla
    uint64_t sw = get_screen_width();
    uint64_t sh = get_screen_height();
  

    // 4) medir cosas
    char buf[64];

    uint64_t fps  = do_benchmark_fps();
    uint64_t hw   = do_benchmark_hardware_access();
    uint64_t flt  = do_benchmark_floating_point();

    //5) Titulo
    print_centered_line("SYSTEM BENCHMARK", sw, 3, 0xFFFFFFFF, 0x000000, size, false);

    // 6) armar y mostrar cada línea
    // FPS
    uintToBase(fps, buf, 10);
    // "FPS: 12345"
    char line_fps[64] = "FPS: ";
    // concateno: como no tenés strcat acá, lo hacemos manual
    {
        char *p = line_fps;
        while (*p) p++;
        char *q = buf;
        while (*q) *p++ = *q++;
        *p = '\0';
    }
    print_centered_line(line_fps, sw, 5, 0xFFFFFF, 0x000000,size, false);

    // Hardware access
    uintToBase(hw, buf, 10);
    char line_hw[64] = "HW ACCESS: ";
    {
        char *p = line_hw;
        while (*p) p++;
        char *q = buf;
        while (*q) *p++ = *q++;
        *p = '\0';
    }
    print_centered_line(line_hw, sw, 7, 0xFFFFFF, 0x000000,size, false);

    // Floating point
    uintToBase(flt, buf, 10);
    char line_flt[64] = "FLOAT: ";
    {
        char *p = line_flt;
        while (*p) p++;
        char *q = buf;
        while (*q) *p++ = *q++;
        *p = '\0';
    }
    print_centered_line(line_flt, sw, 9, 0xFFFFFF, 0x000000,size, false);

    
    print_centered_line("Press Enter to continue", sw, 10, 0xFFFFFF, 0x000000, size, false);
    present_fullframe();
    do_resize("1");
    //HAY QUE BLOQUEAR QUE PUEDA ESCRIBIR 
    //HAY QUE HACER UNA FUNCION QUE ESPERE A UN ENTER que sea generica es la idea 
    clearwindow(0x00000000);
}
