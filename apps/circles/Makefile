CCFLAGS += -DCOCO
CCFLAGS += --coco # Coco .bin file
CCFLAGS += --limit=7800 # protect stack
CCFLAGS += -I../../lib/cc3 -I../../lib/os

CC = cmoc $(CCFLAGS) # --verbose
XROAR = xroar

BASENAME = circles
C_SOURCES = circles.c
TARGETBIN = $(BASENAME).bin
#OBJECT = $(BASENAME).o
#LIBS = -lcmoc-std-ecb -lcmoc-crt-ecb -lcc3

all: $(TARGETBIN)

$(TARGETBIN): $(C_SOURCES)
	$(CC) -L../../lib/cc3 -o $@ $(C_SOURCES) -lcc3

#$(TARGETBIN): $(OBJECT)
#	lwlink -L$(MSYSTEM_PREFIX)/share/cmoc/lib -L../../lib/cc3 -o $@ $(OBJECT) $(LIBS)

clean:
	-rm $(TARGETBIN)
#	-rm $(OBJECT)

run_bin_in_xroar: $(TARGETBIN)
	$(XROAR) $<
