// ---------------------------------------------------------------------
// benchmark.c (userland UI)
// Dibuja una pantalla con los resultados de los benchs del kernel y userland
// ---------------------------------------------------------------------
#include "../include/syscall_call.h"
#include "../utils/utils.h"
#include "../include/benchmark_calculations.h"

void print_benchmark(void) {
    // limpiar pantalla
    clearwindow(0x00000000);

    // agrandar letra
    do_resize("2");

    uint64_t sw = get_screen_width();
    int      size = 16;

    const char *hline = "+--------------------------------------------+";

    // filas (en "celdas" de texto)
    int top_row     = 2;
    int title_row   = top_row + 1;
    int blank_row   = top_row + 2;
    int k_flt_row   = top_row + 3;    // KERNEL FLOAT
    int k_hw_row    = top_row + 4;    // KERNEL PUTPX (HW)
    int u_px_row    = top_row + 5;    // USER PUTPX
    int k_tmr_row   = top_row + 6;    // KERNEL TIMER
    int u_sc_row    = top_row + 7;    // USER SYSCALL TIMER
    int u_mem_row   = top_row + 8;    // USER MEMWRITE
    int bottom_row  = top_row + 9;

    bool running = true;

    while (running) {
        char buf[64];

        // =========================================================
        // 1) MEDICIONES DEL KERNEL
        // =========================================================
        uint64_t k_hw  = do_benchmark_hardware_access();
        uint64_t k_flt = do_benchmark_floating_point();
        uint64_t k_tmr = do_benchmark_timer_latency();


        char line_k_hw[64] = "KERNEL PUTPX (HW): ";
        uintToBase(k_hw, buf, 10);
        {
            char *p = line_k_hw;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

        char line_k_flt[64] = "KERNEL FLOAT: ";
        uintToBase(k_flt, buf, 10);
        {
            char *p = line_k_flt;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

        char line_k_tmr[64] = "KERNEL TIMER: ";
        uintToBase(k_tmr, buf, 10);
        {
            char *p = line_k_tmr;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

        // =========================================================
        // 2) MEDICIONES USERLAND (las de benchmark_calculations.c)
        // =========================================================
        uint64_t u_sc  = syscall_latency();
        uint64_t u_px  = putpixel_user();
        uint64_t u_mem = memwrite_user();

        char line_u_sc[64] = "USER SYSCALL TIMER: ";
        uintToBase(u_sc, buf, 10);
        {
            char *p = line_u_sc;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

        char line_u_px[64] = "USER PUTPX: ";
        uintToBase(u_px, buf, 10);
        {
            char *p = line_u_px;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

        char line_u_mem[64] = "USER MEMWRITE: ";
        uintToBase(u_mem, buf, 10);
        {
            char *p = line_u_mem;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

        // =========================================================
        // 3) DIBUJO DE LA CAJA
        // =========================================================
        print_centered_line(hline, sw, top_row, 0xFFFFFF, 0x000000, size, true);
        print_centered_line("|              SYSTEM BENCHMARK              |", sw, title_row, 0xFFFFFF, 0x000000, size, true);
        print_centered_line("|                                            |", sw, blank_row, 0xFFFFFF, 0x000000, size, true);

        // -------------------- KERNEL FLOAT --------------------
        {
            char line_box[80] = "|                                            |";
            int  box_width = 44;
            int  text_len  = 0;
            while (line_k_flt[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            const char *src = line_k_flt;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, k_flt_row, 0xFFFFFF, 0x000000, size, true);
        }


        // -------------------- KERNEL HW --------------------
        {
            char line_box[80] = "|                                            |";
            int  box_width = 44;
            int  text_len  = 0;
            while (line_k_hw[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            const char *src = line_k_hw;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, k_hw_row, 0xFFFFFF, 0x000000, size, true);
        }

        // -------------------- USER PUTPX --------------------
        {
            char line_box[80] = "|                                            |";
            int  box_width = 44;
            int  text_len  = 0;
            while (line_u_px[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            const char *src = line_u_px;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, u_px_row, 0xFFFFFF, 0x000000, size, true);
        }

        // -------------------- KERNEL TIMER --------------------
        {
            char line_box[80] = "|                                            |";
            int  box_width = 44;
            int  text_len  = 0;
            while (line_k_tmr[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            const char *src = line_k_tmr;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, k_tmr_row, 0xFFFFFF, 0x000000, size, true);
        }

        // -------------------- USER SYSCALL --------------------
        {
            char line_box[80] = "|                                            |";
            int  box_width = 44;
            int  text_len  = 0;
            while (line_u_sc[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            const char *src = line_u_sc;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, u_sc_row, 0xFFFFFF, 0x000000, size, true);
        }

        // -------------------- USER MEMWRITE --------------------
        {
            char line_box[80] = "|                                            |";
            int  box_width = 44;
            int  text_len  = 0;
            while (line_u_mem[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            const char *src = line_u_mem;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, u_mem_row, 0xFFFFFF, 0x000000, size, true);
        }

        print_centered_line(hline, sw, bottom_row, 0xFFFFFF, 0x000000, size, true);

        // mensaje afuera
        do_resize("1");
        write_at_vram("Press any key to stop", 50, 25, 0xFFFFFF, 0x000000);

        // esperar tecla
        char c = getchar();
        if (c != 0) {
            running = false;
        }

        // volver a letra grande para la próxima iteración
        do_resize("2");
    }

    // restaurar
    do_resize("1");
    clearwindow(0x00000000);
}

// funcion aux para dibujar el aviso 
static void draw_fps_warning(uint64_t sw, int size) {
    print_centered_line("+--------------------------------------------+", sw, 6, 0xFFFFFF, 0x000000, size, true);
    print_centered_line("WARNING:", sw, 7, 0xFFFF00, 0x000000, size, true);
    print_centered_line("For one second will be a flash of colors.", sw, 9, 0xFFFFFF, 0x000000, size, true);
    print_centered_line("Press ENTER to start or E to exit", sw, 11, 0x00FFFF, 0x000000, size, true);
    print_centered_line("+--------------------------------------------+", sw, 13, 0xFFFFFF, 0x000000, size, true);
}

// calculo de fps
void fps(void) {
    
    clearwindow(0x000000);
    do_resize("2");
    uint64_t sw = get_screen_width();
    int size = 16;

    // aviso inicial
    draw_fps_warning(sw, size);

    // Primera tecla: esperar ENTER para empezar o E para salir
    char c;
    do {
        do { c = getchar(); } while (c == 0);
        if (c == '\n') break;        // ENTER -> continuar
        if (c == 'e' || c == 'E') {  // E -> salir
            do_resize("1");
            clearwindow(0x000000);
            return;
        }
    } while (true);

    
    while (true) {
        // Ejecutar benchmark
        uint64_t k_fps = do_benchmark_fps();

        // Mostrar resultado
        char numbuf[32];
        uintToBase(k_fps, numbuf, 10);
        char line[64] = "KERNEL FPS: ";
        int p = 0; while (line[p]) p++;
        int q = 0; while (numbuf[q]) line[p++] = numbuf[q++];
        line[p] = '\0';

        clearwindow(0x000000);
        print_centered_line(line, sw, 10, 0x00FFFF, 0x000000, size, true);
        print_centered_line("Press ENTER to calculate again or E to exit", sw, 12, 0xFFFFFF, 0x000000, size, true);

        // Espera un enter o E para ver como sigue 
        char key;
        do {
            do { key = getchar(); } while (key == 0);
            if (key == '\n') break;        // ENTER -> continuar
            if (key == 'e' || key == 'E') { // E -> salir
                do_resize("1");
                clearwindow(0x000000);
                return;
            }
        } while (true);
    }

    //  salida 
    do_resize("1");
    clearwindow(0x000000);
}
