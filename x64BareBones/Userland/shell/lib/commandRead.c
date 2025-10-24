#include <string.h>
#include <ctype.h>
#include "commandRead.h"
#include "commands.h"
extern void println(const char* s);

#ifndef CR_MAXARGS
#define CR_MAXARGS 32
#endif
int   cr_last_cmd_id = -1;
int   cr_last_argc   = 0;
char* cr_last_argv[CR_MAXARGS];

// asumo que ya tenés cr_first_token(...) y cr_match_prefix_range(...)

// --- helpers sin libc ---
static int my_tolower(int c) {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}
static size_t my_strlen(const char* s) {
    size_t n = 0; if (!s) return 0; while (s[n]) n++; return n;
}
static int my_strncmp(const char* a, const char* b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        unsigned char ca = (unsigned char)a[i], cb = (unsigned char)b[i];
        if (ca != cb) return (ca < cb) ? -1 : 1;
        if (ca == 0) return 0;
    }
    return 0;
}


static int simple_tokenize(char* line, char* argv[], int maxargv) {
    int argc = 0;
    char* p = line;
    // saltar espacios
    while (*p==' '||*p=='\t'||*p=='\r'||*p=='\n') p++;
    while (*p && argc < maxargv-1) {
        argv[argc++] = p;
        while (*p && *p!=' ' && *p!='\t' && *p!='\r' && *p!='\n') p++;
        if (*p) { *p = '\0'; p++; }
        while (*p==' '||*p=='\t'||*p=='\r'||*p=='\n') p++;
    }
    argv[argc] = NULL;
    return argc;
}
// --- first token (no modifica el buffer) ---
static int is_space(char c) {
    return c==' ' || c=='\t' || c=='\r' || c=='\n';
}

int cr_first_token(const char* buf, cr_token_t* out) {
    if (!buf || !out) return 0;
    const char* p = buf;
    while (*p && is_space(*p)) p++;
    if (!*p) return 0;
    const char* start = p;
    while (*p && !is_space(*p)) p++;
    out->token = start;
    out->len   = (int)(p - start);
    return 1;
}

// --- prefix matching sobre arreglo ORDENADO COMMANDS[] ---
static int char_at_ci(const char* s, int pos) {
    unsigned char c = (unsigned char)s[pos];
    return c ? my_tolower(c) : -1;
}

static int lb_pref_char(int lo, int hi, int pos, int ch) {
    while (lo < hi) {
        int mid = lo + (hi - lo)/2;
        int cm = char_at_ci(COMMANDS[mid].name, pos);
        if (cm < ch) lo = mid + 1; else hi = mid;
    }
    return lo;
}

static int ub_pref_char(int lo, int hi, int pos, int ch) {
    while (lo < hi) {
        int mid = lo + (hi - lo)/2;
        int cm = char_at_ci(COMMANDS[mid].name, pos);
        if (cm <= ch) lo = mid + 1; else hi = mid;
    }
    return lo;
}

int cr_match_prefix_range(const char* token, int len, cr_match_t* out) {
    if (!token || !out || len <= 0) return 0;
    int lo = 0, hi = N_COMMANDS, pos = 0;
    while (pos < len && lo < hi) {
        int ch = my_tolower((unsigned char)token[pos]);
        int lo2 = lb_pref_char(lo, hi, pos, ch);
        int hi2 = ub_pref_char(lo, hi, pos, ch);
        lo = lo2; hi = hi2; pos++;
        if (lo >= hi) break;
    }
    out->lo = lo; out->hi = hi; out->pos = pos;
    return (hi > lo);
}


void cr_dispatch_exact(char* buf) {
    // valores seguros por defecto
    cr_last_cmd_id = -1;
    cr_last_argc   = 0;
    cr_last_argv[0]= NULL;

    cr_token_t t;
    if (!cr_first_token(buf, &t)) { println("comando invalido"); return; }

    cr_match_t m;
    if (!cr_match_prefix_range(t.token, t.len, &m)) { println("comando invalido"); return; }

    if (m.hi - m.lo != 1) { println("comando invalido"); return; }

    const char* name = COMMANDS[m.lo].name;
    if ((int)my_strlen(name) != t.len || my_strncmp(name, t.token, t.len) != 0) {
        println("comando invalido"); return -1;
    }

    // tokenizar recién ahora (modifica buf)
    int argc = simple_tokenize(buf, cr_last_argv, CR_MAXARGS);

    // dejar “en bandeja”
    cr_last_cmd_id = COMMANDS[m.lo].id;  // tu ID numérico
    cr_last_argc   = argc;  

    commands_Handler(cr_last_cmd_id,cr_last_argc,cr_last_argv);
}

