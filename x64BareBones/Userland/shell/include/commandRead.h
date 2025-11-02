#ifndef COMMANDREAD_H
#define COMMANDREAD_H

#include <stddef.h>

#ifndef CR_MAXARGS
#define CR_MAXARGS 32
#endif

// Primer token (sin modificar el buffer)
typedef struct {
    const char *token;  // Apunta dentro de buf
    int len;            // Longitud del token (sin NUL)
} cr_token_t;

// Resultado del matcheo por prefijo sobre COMMANDS[] (ordenado)
typedef struct {
    int lo;   // Índice inicial candidato (inclusive)
    int hi;   // Índice final candidato (exclusivo)
    int pos;  // Cuántos chars del token se consumieron
} cr_match_t;

// Variables “en bandeja” (último parse/match exitoso)
extern int cr_last_cmd_id;             // ID numérico del comando
extern int cr_last_argc;               // Cantidad de argumentos
extern char *cr_last_argv[CR_MAXARGS]; // argv sobre el mismo buffer

// API principal
int cr_dispatch_exact(char *buf);

#endif // COMMANDREAD_H
