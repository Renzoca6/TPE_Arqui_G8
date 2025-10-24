#include "commands.h"
#include "../lib/syscall_call.h"

// —— declaraciones mínimas para que compile ——
extern void syscall_clear(void);                         // <-- prototipo
int clear(void);                                         // <-- firma sin args
int echo(int argc, char *argv[]);                       // <-- firma con args
int help(const command_t *comandos, int n);             // <-- pasa también la cantidad

// ⚠️ Mantené este arreglo ORDENADO alfabéticamente por 'name'
const command_t COMMANDS[] = {
    { "clear", 0 },
    { "echo",  1 },
    { "help",  2 },
};

const int N_COMMANDS = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

void commands_Handler(int func, int argc, char *argv[]) {
    switch (func) {
        case 0: clear();                       break;
        case 1: echo(argc, argv);              break;
        case 2: help(COMMANDS, N_COMMANDS);    break;
        default:                               break;
    }
}

int clear(void) {
    return 0;                                            // <-- devolver algo
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
