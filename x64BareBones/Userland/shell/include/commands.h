#ifndef COMMANDS_H
#define COMMANDS_H

// Firma estándar de los handlers de comandos
typedef int (*cmd_handler_t)(int argc, char **argv);

// Par (nombre → función)
typedef struct {
    const char *name;  // Nombre del comando (ej: "help")
    int id;            // ID del comando
} command_t;

// Tabla de comandos (definida en commands.c)
extern const command_t COMMANDS[];
extern const int N_COMMANDS;

// Dispatcher de comandos
int commands_Handler(int func, int argc, char *argv[]);

#endif // COMMANDS_H
