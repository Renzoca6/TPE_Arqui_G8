// ---------------------------------------------------------------------
// commands.c
// Tabla de comandos + dispatcher principal del shell
// IMPORTANTE: la tabla debe estar en orden alfabético por nombre
// ---------------------------------------------------------------------
#include "../include/commands.h"
#include "../include/syscall_call.h"
#include "../include/help.h"
#include "../utils/utils.h"
#include "../include/benchmark.h"
#include "../include/tron_game.h"


void clear(void);
void echo(int argc, char *argv[]);
void fps(void);
int  help(const command_t *comandos, int n, int argc, char *argv[]);
void date(void);
void time(void);
void benchmark(void);
void testInvalidOpcode(void);
void testZeroDivision(void);
extern void throw_zero_division(void);
void resize(int argc, char *argv[]);
extern void throw_invalid_opcode(void);
extern void sys_kill_system(void);
void tron(void);
void registers(void);
void kill(void);
void testSound(void);
void sleep(int argc, char *argv[]);
void testsyscalls(void);  

// ---------------------------------------------------------------------
// IMPORTANTE: DEBE SER EN ORDEN ALFABÉTICO
// ---------------------------------------------------------------------
const command_t COMMANDS[] = {
    { "benchmark",  8 },
    { "clear",      0 },
    { "date",       1 },
    { "echo",       2 },
    { "fps",        15 },
    { "help",       3 },
    { "kill",      11 },
    { "registers",  9 },
    { "resize",     4 },
    { "sleep",     13 },
    { "testinvalidop",     5 },
    { "testsound", 12 },
    { "testsyscalls",14 },  
    { "testzero",   6 },
    { "time",       7 },
    { "tron",      10 }
};

const int N_COMMANDS = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

// ---------------------------------------------------------------------
// Dispatcher por id
// ---------------------------------------------------------------------
int commands_Handler(int func, int argc, char *argv[]) {
    switch (func) {
        case 0:  clear();                                break;
        case 1:  date();                                 break; 
        case 2:  echo(argc, argv);                       break;
    case 3:  help(COMMANDS, N_COMMANDS, argc, argv); break;
    case 15: fps();                                   break;
        case 4:  resize(argc, argv);                     break;   
        case 5:  testInvalidOpcode();                    break;
        case 6:  testZeroDivision();                     break;
        case 7:  time();                                 break;   
        case 8:  benchmark();                            break; 
        case 9:  registers();                            break;
        case 10: tron();                                 break;
        case 11: kill();                                 break;
        case 12: testSound();                            break;
        case 13: sleep(argc, argv);                      break;
        case 14: testsyscalls();                         break;
        default:                                         break;
    }
    return 0;
}


void sleep(int argc, char *argv[]) {
    if (argc != 2 || !is_numeric(argv[1])) {
        println("Usage: sleep <milliseconds>");
    } else {
        char buf[10];
        uintToBase(get_ms_since_boot(), buf, 10);
        println(buf);

        sleep_ms(string_to_int(argv[1]));

        char buf1[10];
        uintToBase(get_ms_since_boot(), buf1, 10);
        println(buf1);
    }
}


void tron(void) {
    startGame();
}


void resize(int argc, char *argv[]) {
    if (argc != 2 || !is_numeric(argv[1])) {
        println("Usage: resize <1|2|3>");
    } else {
        do_resize(argv[1]);
    }
}


void benchmark(void) {
    print_benchmark();
}

void testSound(void) {
    audio_beep(440, 200);
    sleep_ms(150);

    audio_beep(523, 200);
    sleep_ms(150);

    audio_play(440);
    sleep_ms(400);
    audio_stop();
}


void date(void) {
    get_date();
    println("");
}

void time(void) {
    get_time();
    println("");
}


void clear(void) {
    clearwindow(0x000000);
}


int help(const command_t *comandos, int n, int argc, char *argv[]) {
    return help_impl(comandos, n, argc, argv);
}


void echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        write(argv[i]);
        write(" ");
    }
    write("\n");
}


void testZeroDivision(void) {
    throw_zero_division();
}

void testInvalidOpcode(void) {
    throw_invalid_opcode();
}


void registers(void) {
    printRegisters();
}


void kill(void) {
    println("");
    println("  +----------------------------------------+");
    println("  |                                        |");
    println("  |       SYSTEM SHUTDOWN INITIATED        |");
    println("  |                                        |");
    println("  |   The system will shutdown in 5 sec... |");
    println("  |                                        |");
    println("  +----------------------------------------+");
    println("");
    sleep_ms(5000);
    sys_kill_system();
}



void testsyscalls(void) {
    println("");
    println("+-------------------------------------------+");
    println("|        SYSCALL FUNCTIONALITY TEST         |");
    println("+-------------------------------------------+");

    // Limpieza de pantalla
    println("Testing clearwindow...");
    sleep_ms(1000);
    clearwindow(0x000000);
    sleep_ms(1000);

    // Info de pantalla
    uint64_t w = get_screen_width();
    uint64_t h = get_screen_height();
    println("Screen info OK");

    // Pixel test
    println("Drawing test pixels...");
    putPixel(0x00FFFF, w/2, h/2, 0);
    putPixel(0xFF00FF, w/2 + 10, h/2, 1);
    sleep_ms(1000);

    // Texto test
    println("Writing test text...");
    write_at_vram("VRAM OK", 2, 2, 0xFFFFFF, 0x000000);
    write_at_back("BACK OK", 2, 3, 0xFFFFFF, 0x000000);
    sleep_ms(1000);
    present_fullframe();

    // Tiempo
    println("Testing time syscalls...");
    uint64_t t0 = get_ms_since_boot();
    sleep_ms(1000);
    uint64_t t1 = get_ms_since_boot();
    if (t1 > t0) println("Timer OK");
    else println("Timer FAIL");

    // Audio
    println("Testing audio...");
    audio_beep(440, 200);
    sleep_ms(1000);

    // Benchmark
    println("Testing benchmark syscalls...");
    uint64_t fps  = do_benchmark_fps();
    uint64_t flt  = do_benchmark_floating_point();
    uint64_t hw   = do_benchmark_hardware_access();
    char buf[10];
    uintToBase(fps, buf, 10);
    write("FPS: ");write(buf);println("");
    uintToBase(flt, buf, 10);
    write("FPU: ");write(buf);println("");
    uintToBase(hw, buf, 10);
    write("HW:  ");write(buf);println("");
    println("Benchmarks OK");

    // Registers
    println("Testing register snapshot...");
    printRegisters();

    // Fecha y hora
    println("Testing date/time...");
    get_date();
    get_time();

    println("");
    println("+-------------------------------------------+");
    println("|  ALL SYSCALLS TESTED (VISUAL + FUNCTION)  |");
    println("+-------------------------------------------+");
    println("");
}