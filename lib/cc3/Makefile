CCFLAGS += -DCOCO -Dconst= # Disable const
CCFLAGS += --coco       # Coco .bin file
CCFLAGS += --limit=7800 # protect stack
CCFLAGS += -I../os -I../ray -I../math

CC = cmoc $(CCFLAGS) --verbose
ASM = lwasm
AR = lwar -car
ASOUT = --obj -o
AS = cmoc -c

OBJS = cc3circle.o cc3gfx.o cc3hw.o cc3line.o cc3raster.o cc3rect.o cc3triangle.o cc3util.o dither.o mc6809/fmemset.o

%.o: %.as
	$(ASM) $(AFLAGS) $< $(ASOUT)$@

%.o: %.asm
	$(ASM) $(AFLAGS) $< $(ASOUT)$@

all: libcc3.a

libcc3.a: $(OBJS)
	$(AR) $@ $(OBJS)

clean:
	-rm $(OBJS)
	-rm libcc3.a
