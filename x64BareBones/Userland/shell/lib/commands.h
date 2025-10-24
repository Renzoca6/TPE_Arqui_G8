#pragma once

// Firma estándar de los handlers de comandos:
// argc = cantidad de argumentos, argv = vector de strings (argv[0] = nombre del comando)
typedef int (*cmd_handler_t)(int argc, char** argv);

// Par (nombre → función)
typedef struct {
    const char*     name;   // nombre del comando (ej: "help")
    int  id;     // función que implementa el comando
} command_t;

// Tabla de comandos (definida en commands.c)
extern const command_t COMMANDS[];
extern const int       N_COMMANDS;
