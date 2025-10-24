#ifndef COMMANDREAD_H
#define COMMANDREAD_H

#include <stddef.h>

#ifndef CR_MAXARGS
#define CR_MAXARGS 32
#endif

// Primer token (sin modificar el buffer)
typedef struct {
    const char* token;  // apunta dentro de buf
    int         len;    // longitud del token (sin NUL)
} cr_token_t;

// Resultado del matcheo por prefijo sobre COMMANDS[] (ordenado)
typedef struct {
    int lo;   // índice inicial candidato (inclusive)
    int hi;   // índice final candidato (exclusivo)
    int pos;  // cuántos chars del token se consumieron
} cr_match_t;

// Variables “en bandeja” (último parse/match exitoso)
extern int   cr_last_cmd_id;             // ID numérico del comando
extern int   cr_last_argc;               // cantidad de argumentos
extern char* cr_last_argv[CR_MAXARGS];   // argv sobre el mismo buffer

// API
// Extrae el primer token del buffer NUL-terminado `buf`.
// Devuelve 1 si encontró token, 0 si estaba vacío/solo espacios.
int cr_first_token(const char* buf, cr_token_t* out);

// Calcula el rango [lo,hi) de comandos cuyo nombre tiene como prefijo token[0..len).
// Devuelve 1 si hay al menos un candidato, 0 si no hay coincidencias.
int cr_match_prefix_range(const char* token, int len, cr_match_t* out);

// Matchea exacto y, si coincide, tokeniza y llama a tu handler desde commands.c.
// En caso de error imprime "comando invalido". Devuelve el código que retorne tu handler.
void cr_dispatch_exact(char* buf);

#endif // COMMANDREAD_H
