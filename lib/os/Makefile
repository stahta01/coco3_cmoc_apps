CCFLAGS += -DCOCO
CCFLAGS += --limit=7800 # protect stack

CC = cmoc $(CCFLAGS)
ASM = lwasm
AR = lwar -car
ASOUT = --obj -o
AS = cmoc -c

OBJS = hostcompat.o cstdlib.o math.o 

%.o: %.as
	$(ASM) $(AFLAGS) $< $(ASOUT)$@

%.o: %.asm
	$(ASM) $(AFLAGS) $< $(ASOUT)$@

all:	libos.a

libos.a: $(OBJS)
	$(AR) $@ $(OBJS)

clean:
	-rm $(OBJS)
	-rm libos.a
