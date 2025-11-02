#ifndef HELP_H
#define HELP_H

#include "commands.h"

// Implementación del comando help.
// Recibe la tabla, su tamaño y los argumentos capturados.
int help_impl(const command_t *comandos, int n, int argc, char *argv[]);

#endif // HELP_H
