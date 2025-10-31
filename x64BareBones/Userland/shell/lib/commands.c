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
void tron();

//IMPORTANTE DEBE SER EN ORDEN ALFABETICO
const command_t COMMANDS[] = {
    { "a",  10 },
    { "benchmark",  8 },
    { "clear", 0 },
    { "date",  1 },
    { "echo",  2 },
    { "help",  3 },
    { "registers", 9},
    { "resize",4 },   
    { "testop", 5},
    { "testzero", 6},
    { "time",  7 }
};

const int N_COMMANDS = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

void commands_Handler(int func, int argc, char *argv[]) {
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
        default:                                        break;
    }
}


void tron(){
    startGame();
}


void resize(int argc, char *argv[]){
    if (argc != 2){
        println("comando invalido");
    }
    else{
        return do_resize(argv[1]);
    }
}

void benchmark(){
    print_benchmark();
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