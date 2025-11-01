#include "commands.h"
#include "syscall_call.h"
#include "help.h"
#include "../utils/utils.h"
#include "../commands/benchmark.h"
#include "../commands/tron_game.h"


// —— declaraciones mínimas para que compile ——
extern void syscall_clear(void);                         // <-- prototipo
void clear(void);                                         // <-- firma sin args
void echo(int argc, char *argv[]);                       // <-- firma con args
int help(const command_t *comandos, int n, int argc, char *argv[]);             // <-- pasa también la cantidad
void date();
void time();
void benchmark();  
void testInvalidOpcode();
void testZeroDivision();
extern void throw_zero_division();
void resize(int argc, char *argv[]);
extern void throw_invalid_opcode ();
extern void sys_kill_system();
void tron();
void registers();
void kill();
void testSound();



//IMPORTANTE DEBE SER EN ORDEN ALFABETICO
const command_t COMMANDS[] = {
    { "benchmark",  8 },
    { "clear", 0 },
    { "date",  1 },
    { "echo",  2 },
    { "help",  3 },
    { "kill", 11},
    { "registers", 9},
    { "resize",4 },  
    { "testop", 5},
    {"testsound", 12}, 
    { "testzero", 6},
    { "time",  7 },
    { "tron", 10}
};

const int N_COMMANDS = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

int commands_Handler(int func, int argc, char *argv[]) {
    switch (func) {
        case 0: clear();                                break;
        case 1: date();                                 break; 
        case 2: echo(argc,argv);                        break;
        case 3: help(COMMANDS, N_COMMANDS, argc, argv); break;
        case 4: resize(argc,argv);                      break;   
        case 5: testInvalidOpcode();                    break;
        case 6: testZeroDivision();                     break;
        case 7: time();                                 break;   
        case 8: benchmark();                            break; 
        case 9: registers();                            break;
        case 10: tron();                                break;
        case 11: kill(); return 1;                      break;
        case 12: testSound();                           break;
        default:                                        break;
    }
    return 0;
}


void tron(){
    startGame();
}


void resize(int argc, char *argv[]){
    if (argc != 2){
        println("invalid command");
    }
    else{
        return do_resize(argv[1]);
    }
}

void benchmark(){
    print_benchmark();
}


void testSound(){
    audio_beep(440, 200);
    sleep_ms(150);

    audio_beep(523, 200);
    sleep_ms(150);

    audio_play(440);
    sleep_ms(400);
    audio_stop();
}

void date(){
    get_date();
    println("");
}

void time(){
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
    for (int i = 1 ; i < argc; i++){
        write(argv[i]);
        write(" ");
    }
    write("\n");
}

void testZeroDivision() {
    throw_zero_division();
}

void testInvalidOpcode() {
    throw_invalid_opcode();
}

void registers(){
    printRegisters();
}

void kill(){
    println("");
    println("         ______");
    println("         |    |");
    println("         |    O");
    println("         |   /|\\");
    println("         |   / \\");
    println("         |");
    println("        _|_");
    println("");
    println("          SYSTEM SHUTDOWN");
    println("  The system has been terminated.");
    println("");
    sys_kill_system();
}