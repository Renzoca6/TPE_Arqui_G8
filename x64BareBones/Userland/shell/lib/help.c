#include "help.h"

// Traemos solo lo que necesitamos del entorno:
extern void println(const char* s);

// ---- helpers locales (sin libc) ----
static int my_tolower_(int c){ return (c>='A'&&c<='Z') ? c+('a'-'A') : c; }
static int ci_strcmp_(const char* a, const char* b){
    for (;; a++, b++){
        unsigned char ca = (unsigned char)*a, cb = (unsigned char)*b;
        int da = my_tolower_(ca), db = my_tolower_(cb);
        if (da != db) return (da < db) ? -1 : 1;
        if (ca == 0)  return 0; // llegaron ambos al '\0' => iguales
    }
}

// ---- diccionario de ayuda (nombre -> desc/uso) ----
typedef struct { const char* name; const char* desc; const char* usage; } help_entry_t;

static const help_entry_t HELP_ENTRIES[] = {
    { "clear", "Limpia la pantalla.",                "clear" },
    { "date",  "Muestra la fecha del sistema.",      "date" },
    { "echo",  "Imprime los argumentos recibidos.",  "echo [args...]" },
    { "help",  "Muestra ayuda de comandos.",         "help [comando]" },
    { "time",  "Muestra la hora del sistema.",       "time" },
};

// Requiere: HELP_ENTRIES[] ORDENADO case-insensitive por 'name'
static const help_entry_t* find_help_entry_(const char* name){
    int lo = 0;
    int hi = (int)(sizeof(HELP_ENTRIES)/sizeof(HELP_ENTRIES[0]));
    while (lo < hi){
        int mid = lo + (hi - lo)/2;
        int c = ci_strcmp_(HELP_ENTRIES[mid].name, name);
        if (c == 0) return &HELP_ENTRIES[mid];   // ¡encontrado!
        if (c < 0) lo = mid + 1;                 // name está "después"
        else       hi = mid;                     // name está "antes"
    }
    return 0; // no está
}

static void help_list_all_(const command_t* comandos, int n){
    println("Comandos disponibles:");
    for (int i = 0; i < n; i++){
        const char* name = comandos[i].name;
        const help_entry_t* h = find_help_entry_(name);
        println(name);
        if (h) println(h->desc);
    }
}

static void help_one_(const char* name){
    const help_entry_t* h = find_help_entry_(name);
    if (!h){ println("help: comando desconocido"); return; }
    println("Comando:");
    println(h->name);
    println("Descripcion:");
    println(h->desc);
    println("Uso:");
    println(h->usage);
}

// API del módulo
int help_impl(const command_t *comandos, int n, int argc, char *argv[]){
    // help <comando>
    if (argc >= 2 && argv[1] && argv[1][0]) {
        help_one_(argv[1]);
        return 0;
    }
    // help
    help_list_all_(comandos, n);
    return 0;
}
