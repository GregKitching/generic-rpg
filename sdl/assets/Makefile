CPP = g++
CFLAGS = -03
LDFLAGS =
DEPS =
OBJ = main.o

sdl_cflags := $(shell pkg-config --cflags sdl2 SDL2_mixer SDL2_image)
sdl_libs := $(shell pkg-config --libs sdl2 SDL2_mixer SDL2_image)
override CFLAGS += $(sdl_cflags)
override LIBS += $(sdl_libs)

%.o: %.cpp $(DEPS)
	$(CPP) $(LDFLAGS) -c -o $@ $< $(LIBS)

main: $(OBJ)
	$(CPP) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f main *.o
.PHONY: all clean
