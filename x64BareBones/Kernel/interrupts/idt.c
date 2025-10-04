#include "idt.h"
#include <stdint.h>
#include <interrupts.h>

#define IDT_ENTRIES 256

// Descripción de una entrada en la IDT
struct idt_entry {
    uint16_t offset_low;     // bits 0..15 de la dirección
    uint16_t selector;       // segmento de código (usualmente 0x08 en BareBones)
    uint8_t  ist;            // stack table (no usamos, =0)
    uint8_t  type_attr;      // tipo y flags (0x8E = interrupt gate)
    uint16_t offset_mid;     // bits 16..31 de la dirección
    uint32_t offset_high;    // bits 32..63 de la dirección
    uint32_t zero;           // reservado = 0
} __attribute__((packed));

// Descriptor de la IDT (lo que carga lidt)
struct idt_descriptor {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_descriptor idt_desc;

extern void load_idt(void*);   // implementado en ASM


// Crea una entrada en la IDT
void idt_set_entry(int i, uint64_t handler) {
    //capos que pide intel
    idt[i].selector    = 0x08;       // segmento de código kernel
    idt[i].ist         = 0;
    idt[i].type_attr   = 0x8E;       // present, ring0, interrupt gate
    idt[i].zero        = 0;

    //La direccion de la funcion se guarda en 3 partes 
    idt[i].offset_low  = handler & 0xFFFF;
    idt[i].offset_mid  = (handler >> 16) & 0xFFFF;
    idt[i].offset_high = (handler >> 32) & 0xFFFFFFFF;
}

// Inicializa la IDT
void idt_init(void) {
    idt_desc.limit = sizeof(idt) - 1;          // tamanio de la tabla en memoria
    idt_desc.base  = (uint64_t)&idt;        // puntero a donde empieza el idt

    //cargo a la tabla mi timer
    idt_set_entry(0x20, (uint64_t)&_irq00Handler);
    idt_set_entry(0x21, (uint64_t)&_irq01Handler);
    // idt_set_entry(0x00, (uint64_t)&_exception0Handler); //
    
    // llamo a funcion de asm que manda la direccion de memoria donde esta el idt_desc que es el foramto que me piden para la tabla IDT  a LIDT que carga la tabla
    load_idt(&idt_desc);    
}
