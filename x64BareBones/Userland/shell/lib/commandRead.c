// ---------------------------------------------------------------------
// commandRead.c
// Lectura, tokenización y despacho de comandos en modo texto
// Usa búsqueda binaria por prefijo (case-insensitive) sobre COMMANDS[]
// ---------------------------------------------------------------------
#include <string.h>
#include "../include/commandRead.h"
#include "../include/commands.h"

extern void println(const char *s);

#ifndef CR_MAXARGS
#endif

int   cr_last_cmd_id = -1;
int   cr_last_argc   = 0;
char *cr_last_argv[CR_MAXARGS];

// ---------------------------------------------------------------------
// Helpers sin libc (compatibles bare-metal)
// ---------------------------------------------------------------------
static int my_tolower(int c) {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

static size_t my_strlen(const char *s) {
    size_t n = 0;
    if (!s) return 0;
    while (s[n]) n++;
    return n;
}

// ---------------------------------------------------------------------
// Tokenización simple (destructiva): separa por espacio/tab/CR/LF
// ---------------------------------------------------------------------
static int simple_tokenize(char *line, char *argv[], int maxargv) {
    int   argc = 0;
    char *p    = line;

    // saltar espacios iniciales
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
        p++;

    while (*p && argc < maxargv - 1) {
        argv[argc++] = p;

        // avanzar hasta separador
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n')
            p++;

        // si hay separador, lo corto
        if (*p) {
            *p = '\0';
            p++;
        }

        // skipeo espacios de nuevo
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
            p++;
    }

    argv[argc] = NULL;
    return argc;
}

// ---------------------------------------------------------------------
// Primer token (no modifica el buffer)
// ---------------------------------------------------------------------
static int is_space_(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

static int cr_first_token(const char *buf, cr_token_t *out) {
    if (!buf || !out) return 0;

    const char *p = buf;

    while (*p && is_space_(*p)) p++;
    if (!*p) return 0;

    const char *start = p;

    while (*p && !is_space_(*p)) p++;

    out->token = start;
    out->len   = (int)(p - start);
    return 1;
}

// ---------------------------------------------------------------------
// Búsqueda binaria por prefijo (CI) + verificación de nombre completo (CI).
// Requiere COMMANDS[] ordenado lexicográficamente "case-insensitive".
// ---------------------------------------------------------------------
// compare NAME vs TOKEN en los primeros 'len' chars (case-insensitive):
//   <0 si NAME es "menor" o se queda corto,
//    0 si NAME comienza con TOKEN,
//   >0 si NAME es "mayor" en el primer char que difiere.
static int ci_cmp_prefix(const char *name, const char *tok, int len) {
    for (int i = 0; i < len; i++) {
        unsigned char a = (unsigned char)name[i];
        unsigned char b = (unsigned char)tok[i];

        if (a == 0) return -1;          // name terminó → menor

        int da = my_tolower(a);
        int db = my_tolower(b);

        if (da != db)
            return (da < db) ? -1 : 1;
    }
    return 0;                           // prefijo coincide
}

// lower_bound: primer índice con cmp >= 0
static int lb_prefix(int lo, int hi, const char *tok, int len) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        int c   = ci_cmp_prefix(COMMANDS[mid].name, tok, len);
        if (c < 0)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

// upper_bound: primer índice con cmp > 0
static int ub_prefix(int lo, int hi, const char *tok, int len) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        int c   = ci_cmp_prefix(COMMANDS[mid].name, tok, len);
        if (c <= 0)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

// comparación CI exacta de longitud fija
static int ci_strncmp(const char *a, const char *b, int n) {
    for (int i = 0; i < n; i++) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[i];
        int           da = my_tolower(ca);
        int           db = my_tolower(cb);

        if (da != db) return (da < db) ? -1 : 1;
        if (ca == 0 || cb == 0) return 0;
    }
    return 0;
}

// ---------------------------------------------------------------------
// Devuelve índice si hay EXACTAMENTE 1 candidato por prefijo (CI)
// y además el token coincide COMPLETO (misma longitud) en CI.
// En otro caso, devuelve -1.
// ---------------------------------------------------------------------
static int cr_find_cmd_idx_ci_exact(const char *token, int len) {
    if (!token || len <= 0) return -1;

    int lo = lb_prefix(0, N_COMMANDS, token, len);
    int hi = ub_prefix(0, N_COMMANDS, token, len);

    if (hi - lo != 1) return -1;               // 0 o >1 → inválido/ambiguo

    const char *name    = COMMANDS[lo].name;
    int         namelen = (int)my_strlen(name);

    // exigir nombre completo (no abreviaturas)
    if (namelen != len) return -1;

    // exigir igualdad CI completa
    if (ci_strncmp(name, token, len) != 0) return -1;

    return lo;     // índice del comando
}

// ---------------------------------------------------------------------
// Punto de entrada: lee primer token, busca, tokeniza y despacha
// ---------------------------------------------------------------------
int cr_dispatch_exact(char *buf) {
    // estado seguro por defecto
    cr_last_cmd_id  = -1;
    cr_last_argc    = 0;
    cr_last_argv[0] = NULL;

    // 1) primer token (sin alterar el buffer)
    cr_token_t t;
    if (!cr_first_token(buf, &t)) {
        println("Invalid command. Type \"help\" to check available commands.");
        return 0;
    }

    // 2) buscar índice (CI, binario, único + nombre completo)
    int idx = cr_find_cmd_idx_ci_exact(t.token, t.len);
    if (idx < 0) {
        println("Invalid command. Type \"help\" to check available commands.");
        return 0;
    }

    // 3) tokenizar (alterando el buffer) y despachar
    int argc = simple_tokenize(buf, cr_last_argv, CR_MAXARGS);
    cr_last_cmd_id = COMMANDS[idx].id;
    cr_last_argc   = argc;

    return commands_Handler(cr_last_cmd_id, cr_last_argc, cr_last_argv);
}
