include ../Makefile.inc

MODULE=0000-shell.bin

SOURCES_C  := $(wildcard [^_]*.c) $(wildcard **/*.c)
SOURCES_ASM:= $(wildcard [^_]*.asm) $(wildcard **/*.asm)

# Todos los objetos
OBJECTS_C   := $(SOURCES_C:.c=.o)
OBJECTS_ASM := $(SOURCES_ASM:.asm=.o)
OBJECTS     := $(OBJECTS_C) $(OBJECTS_ASM)

all: $(MODULE)

$(MODULE): $(OBJECTS)
	$(GCC) $(GCCFLAGS) -T shell.ld _loader.c $(OBJECTS) -I./lib -I./commands -o ../$(MODULE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./lib -I./commands -c $< -o $@

%.o: %.asm
	nasm -f elf64 $< -o $@

clean:
	rm -rf *.o

.PHONY: all clean print