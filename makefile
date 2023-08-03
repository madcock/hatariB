CC=gcc
CFLAGS=-O2 -Wall -Werror
LDFLAGS=-shared -Wall -Werror

BD=build/
CORE=$(BD)hatarib.dll
SOURCES = \
	core/core.c \
	core/core_input.c
OBJECTS = $(SOURCES:%.c=$(BD)%.o)
HATARILIBS= \
	hatari/build/src/libcore.a \
	hatari/build/src/falcon/libFalcon.a \
	hatari/build/src/cpu/libUaeCpu.a \
	hatari/build/src/gui-sdl/libGuiSdl.a \
	hatari/build/src/libFloppy.a \
	hatari/build/src/debug/libDebug.a \
	hatari/build/src/libcore.a \
	-lSDL2 \
	-lz
# note: libcore is linked twice to allow other hatari internal libraries to resolve references within it.

default: $(CORE)

directories:
	mkdir -p $(BD)
	mkdir -p $(BD)core
	mkdir -p hatari/build

$(CORE): directories hatarilib $(OBJECTS)
	$(CC) -o $(CORE) $(LDFLAGS) $(OBJECTS) $(HATARILIBS)

$(BD)core/%.o: core/%.c
	$(CC) -o $@ $(CFLAGS) -c $< 

hatarilib: directories
	(cd hatari/build && cmake .. -DENABLE_SMALL_MEM=0 -DENABLE_TRACING=0)
	(cd hatari/build && cmake --build . -j)

clean:
	rm -f -r $(BD)
	rm -f -r hatari/build
