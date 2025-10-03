include Makefile.inc

KERNEL       := kernel.bin

# ---- fuentes ----
C_SRCS       := $(wildcard *.c) \
                $(wildcard drivers/*.c) \
				$(wildcard syscall/*.c) \
                $(wildcard interrupts/*.c)

ASM_SRCS     := $(wildcard asm/*.asm)

C_OBJS       := $(C_SRCS:.c=.o)
ASM_OBJS     := $(ASM_SRCS:.asm=.o)

LOADER       := loader.asm
LOADER_OBJ   := $(LOADER:.asm=.o)

STATICLIBS   :=
INCLUDES     := -I./include

# ---- reglas ----
all: $(KERNEL)

$(KERNEL): $(LOADER_OBJ) $(C_OBJS) $(ASM_OBJS) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^

%.o: %.c
	$(GCC) $(GCCFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADER_OBJ): $(LOADER)
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(C_OBJS) $(ASM_OBJS) $(LOADER_OBJ) $(KERNEL)

.PHONY: all clean
