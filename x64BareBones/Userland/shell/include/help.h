#ifndef HELP_H
#define HELP_H

#include "commands.h"

// Implementación del comando help en un módulo aparte.
// Recibe la tabla, su tamaño, y los args capturados por el dispatcher.
int help_impl(const command_t *comandos, int n, int argc, char *argv[]);

#endif
