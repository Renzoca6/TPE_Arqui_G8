#include "syscall_call.h"
#include "../utils/utils.h"

void print_benchmark(void) {
    clearwindow(0x00000000);

    // agrandar letra
    int size = 16;
    do_resize("2");

    uint64_t sw = get_screen_width();

    const char *hline = "+--------------------------------------------+";

    int top_row    = 2;
    int title_row  = top_row + 1;
    int fps_row    = top_row + 3;
    int hw_row     = top_row + 4;
    int flt_row    = top_row + 5;
    int bottom_row = top_row + 6;

    bool running = true;

    while (running) {
        // medir
        char buf[64];
        uint64_t fps  = do_benchmark_fps();
        uint64_t hw   = do_benchmark_hardware_access();
        uint64_t flt  = do_benchmark_floating_point();

        // armar strings
        char line_fps[64] = "FPS: ";
        uintToBase(fps, buf, 10);
        {
            char *p = line_fps;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

        char line_hw[64] = "HW ACCESS: ";
        uintToBase(hw, buf, 10);
        {
            char *p = line_hw;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

        char line_flt[64] = "FLOAT: ";
        uintToBase(flt, buf, 10);
        {
            char *p = line_flt;
            while (*p) p++;
            char *q = buf;
            while (*q) *p++ = *q++;
            *p = '\0';
        }

        // caja
        print_centered_line(hline, sw, top_row, 0xFFFFFF, 0x000000, size, true);
        print_centered_line("|              SYSTEM BENCHMARK              |", sw, title_row, 0xFFFFFF, 0x000000, size, true);
        print_centered_line("|                                            |", sw, top_row + 2, 0xFFFFFF, 0x000000, size, true);

        // FPS
        {
            char line_box[80] = "|                                            |";
            int box_width = 44;
            int text_len = 0;
            while (line_fps[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            char *src = line_fps;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, fps_row, 0xFFFFFF, 0x000000, size, true);
        }

        // HW
        {
            char line_box[80] = "|                                            |";
            int box_width = 44;
            int text_len = 0;
            while (line_hw[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            char *src = line_hw;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, hw_row, 0xFFFFFF, 0x000000, size, true);
        }

        // FLOAT
        {
            char line_box[80] = "|                                            |";
            int box_width = 44;
            int text_len = 0;
            while (line_flt[text_len]) text_len++;
            int start = (box_width - text_len) / 2;
            char *dst = line_box + 1 + start;
            char *src = line_flt;
            while (*src) *dst++ = *src++;
            print_centered_line(line_box, sw, flt_row, 0xFFFFFF, 0x000000, size, true);
        }

        print_centered_line(hline, sw, bottom_row, 0xFFFFFF, 0x000000, size, true);

        // mensaje afuera
        do_resize("1");
        write_at_vram("Press any key to stop", 50, 18, 0xFFFFFF, 0x000000);

        //present_fullframe();

        // detectar tecla
        char c = getchar();
        if (!(c == 0))
            running = false;

        // restaurar tamaño para siguiente iteración
        do_resize("2");
    }
    do_resize("1");

    clearwindow(0x00000000);

}
