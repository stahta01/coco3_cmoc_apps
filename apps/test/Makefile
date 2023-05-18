CCFLAGS += -DCOCO
CCFLAGS += --coco       # Coco .bin file
CCFLAGS += --limit=7800 # protect stack
CCFLAGS += -I../../lib/os -I../../lib/cc3

CC = cmoc $(CCFLAGS)
ASM = lwasm
AR = lwar -car
ASOUT = --obj -o
AS = cmoc -c

OBJS = printf.o 

%.o: %.as
	$(ASM) $(AFLAGS) $< $(ASOUT)$@

%.o: %.asm
	$(ASM) $(AFLAGS) $< $(ASOUT)$@

all: printf

printf: $(OBJS)
	lwlink -L$(MSYSTEM_PREFIX)/share/cmoc/lib -L../../lib/cc3 -L../../lib/os -o $@ $(OBJS) -lcmoc-std-ecb -lcmoc-crt-ecb -lcc3

clean:
	-rm $(OBJS)
	-rm printf
