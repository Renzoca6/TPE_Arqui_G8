#include "pic.h"
#include <stdint.h>
#include "io.h"
// Puertos del PIC
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

void pic_init(void) {
    // ya está remapeado (el BareBones lo suele hacer supuestamente)
}

// Habilita (desenmascara) una IRQ específica en el PIC 8259A.
// irq: número de línea IRQ global [0..15]. 0–7 = PIC maestro, 8–15 = PIC esclavo.
void pic_unmask_irq(uint8_t irq) {
    uint16_t port;   // Puerto I/O del registro de MÁSCARA (IMR) del PIC a tocar (0x21 u 0xA1)
    uint8_t value;   // Byte temporal para leer/modificar la máscara actual

    // me fijo si es maestro o esclavo
    if (irq < 8) { //maestro
        port = PIC1_DATA; 
    } else {
        port = PIC2_DATA;       
        irq -= 8;           // ya se que es el pic esclavo pero lo debo bajar a entre 0 y 7 (El pic tiene 8 puertos)
    }

    // obtengo como esta el puerto ahora, por ejmplo 
    //      0b1111_1111 y supongamos que irq es 1 --> (1 << irq) = 0b0000_0010 y ~ --> 0b1111_1101
    // entoces 0b1111_1111 & 0b1111_1101 = 0b1111_1101 lo cual indica que el puerto 1 va a estar desenmascarado 
    value = inb(port) & ~(1 << irq);
   
    //cargo el valor que acabo de calcular en el puerto 
    outb(port, value);
}

// razonamiento similar 
void pic_mask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = 0xA1; 
        irq -= 8;
    }

    value = inb(port) | (uint8_t)(1u << irq);  
    outb(port, value);
}