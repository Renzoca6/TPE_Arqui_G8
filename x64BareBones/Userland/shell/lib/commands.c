#include "commands.h"
#include "syscall_call.h"


// —— declaraciones mínimas para que compile ——
extern void syscall_clear(void);                         // <-- prototipo
void clear(void);                                         // <-- firma sin args
int echo(int argc, char *argv[]);                       // <-- firma con args
int help(const command_t *comandos, int n);             // <-- pasa también la cantidad
void date();
void time();

//IMPORTANTE DEBE SER EN ORDEN ALFABETICO
const command_t COMMANDS[] = {
    { "clear", 0 },
    { "echo",  1 },
    { "date",  3 },
    { "help",  2 },
    { "time",  4 },

};

const int N_COMMANDS = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

void commands_Handler(int func, int argc, char *argv[]) {
    switch (func) {
        case 0: clear();                       break;
        case 1: echo(argc, argv);              break;
        case 2: help(COMMANDS, N_COMMANDS);    break;
        case 3: date();                        break; 
        case 4: time();                        break;   
        default:                               break;
    }
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
    clearwindow(0x00F000);
}

int help(const command_t *comandos, int n) {
    // TODO: implementar con 'comandos' y 'n'
    // (ej: listar nombres)
    (void)comandos; (void)n;
    return 0;
}

int echo(int argc, char *argv[]) {
    write("nigger");
}
