TARGET = bbox.elf
OBJS = main.o romdisk.o
KOS_ROMDISK_DIR = romdisk
KOS_CFLAGS += -I${KOS_PORTS}/include/raylib

all: $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean: rm-elf
	-rm -f $(OBJS)

rm-elf:
	-rm -f $(TARGET) romdisk.*

$(TARGET): $(OBJS)
	kos-c++ -o $(TARGET) $(OBJS) -lraylib -lGL -lm -lkosutils -lADX

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist: $(TARGET)
	-rm -f $(OBJS) romdisk.img
	$(KOS_STRIP) $(TARGET)

.PHONY: all clean rm-elf run dist