#include "./lib/syscall_call.h"
#include "./lib/commandRead.h"
#include "./tron2/include/map.h"
#include "./tron2/include/types.h"



int main() {

    TronGame game;

    // 1) inicializamos
    map_init(&game);

    // 2) dibujamos la grilla que dejó el init
    // color gris oscuro, target = 0 (cambiá si tu enum es otro)
    map_draw_grid_lines(&game, 1);

    for (int i = 0; i < 10; i++){
        map_draw_cell(&game, i, 0, 0x0FF000, 1);
    }

    //present_fullframe();
    
  
    char buf[256];
    while (1){
        write("- ");
        read(buf);
        cr_dispatch_exact(buf);}
    return 1;
}
