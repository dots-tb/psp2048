TARGET:= 2048-PSP
TITLE:= 2048000001

OBJS = src/psp2048.o graphics/bg.o graphics/b0.o graphics/b1.o graphics/b2.o graphics/b3.o graphics/numtable.o \
	graphics/0002.o graphics/0004.o graphics/0008.o graphics/0016.o \
	graphics/0032.o graphics/0064.o graphics/0128.o graphics/0256.o \
	graphics/0512.o graphics/1024.o graphics/2048.o graphics/bg.o graphics/numtable.o \


PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = -Wl,-q -Wall -O3
CXXFLAGS  = $(CFLAGS) -fno-exceptions
ASFLAGS = $(CFLAGS)


LIBDIR =
LDFLAGS = 
LIBS = -lvita2d -lSceKernel_stub -lSceTouch_stub -lSceDisplay_stub -lSceGxm_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lScePgf_stub \
	-lSceCommonDialog_stub -lfreetype -lpng -ljpeg -lz -lm -lc

eboot.bin: $(TARGET).velf
	vita-make-fself $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.png
	$(PREFIX)-ld -r -b binary -o $@ $^

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS)

vpk: $(TARGET).velf
	vita-make-fself $< eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE) "$(TARGET)" param.sfo
	cp -f param.sfo sce_sys/param.sfo
	
	#------------ Comment this if you don't have 7zip ------------------
	7z a -tzip $(TARGET).vpk -r sce_sys/* eboot.bin 
	#-------------------------------------------------------------------