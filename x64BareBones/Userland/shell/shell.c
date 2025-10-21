#include "./lib/syscall_call.h"

int main() {
    char hola[] = "TEST1 ARREGLO";

    write(hola);
    write("TEST2 CHAR*");

	return 0;
}
