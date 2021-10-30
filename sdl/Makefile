CPP = g++
CFLAGS = -03
LDFLAGS =
name = something

all: $(name)
clean:
	rm -f $(name) *.o
.PHONY: all clean

sdl_cflags := $(shell pkg-config --cflags sdl2 SDL2_mixer SDL2_image)
sdl_libs := $(shell pkg-config --libs sdl2 SDL2_mixer SDL2_image)
override CFLAGS += $(sdl_cflags)
override LIBS += $(sdl_libs)

$(name): $(name).o
	$(CPP) $(LDFLAGS) -o $@ $^ $(LIBS)
