global load_idt
GLOBAL enable_interrupts
section .text

load_idt:
    ;lidt es una instrucción de la CPU x86/x86-64 que carga el registro IDTR (Interrupt Descriptor Table Register) con la dirección (base) y el tamaño (limit)
    ; de tu tabla de interrupciones (IDT). Sin IDT cargada, el CPU no sabe a qué handler saltar cuando ocurre una excepción/IRQ.

    lidt [rdi]    ; carga IDT desde dirección pasada en RDI
    ret


enable_interrupts:
    sti
    ret