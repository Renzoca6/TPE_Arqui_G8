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
    int k_fps_row   = top_row + 4;    // KERNEL FPS
    int k_hw_row    = top_row + 5;    // KERNEL PUTPX (HW)
    int u_px_row    = top_row + 6;    // USER PUTPX
    int k_tmr_row   = top_row + 7;    // KERNEL TIMER
    int u_sc_row    = top_row + 8;    // USER SYSCALL TIMER
    int u_mem_row   = top_row + 9;    // USER MEMWRITE
    int bottom_row  = top_row + 10;

    bool running = true;

    while (running) {
        char buf[64];

        // =========================================================
        // 1) MEDICIONES DEL KERNEL
        // =========================================================
        uint64_t k_fps = do_benchmark_fps();
        uint64_t k_hw  = do_benchmark_hardware_access();
        uint64_t k_flt = do_benchmark_floating_point();
        uint64_t k_tmr = do_benchmark_timer_latency();

        char line_k_fps[64] = "KERNEL FPS: ";
        uintToBase(k_fps, buf, 10);
        {
            char *p = line_k_fps;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

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

        // -------------------- KERNEL FPS --------------------
        {
            char line_box[80] = "|                                            |";
            int  box_width = 44;
            int  text_len  = 0;
            while (line_k_fps[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            const char *src = line_k_fps;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, k_fps_row, 0xFFFFFF, 0x000000, size, true);
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
