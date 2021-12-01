CPP = g++
CFLAGS = -03
LDFLAGS =
DEPS = constants.h enums.h SpriteSheet.h Tile.h TileSet.h Map.h Entity.h ActiveEntity.h TextBox.h SpeechTextBox.h MenuTextBox.h BookTextBox.h Script.h SoundHandler.h
OBJ = main.o SpriteSheet.o Tile.o TileSet.o Map.o Entity.o ActiveEntity.o TextBox.o SpeechTextBox.o MenuTextBox.o BookTextBox.o Script.o SoundHandler.o

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
#-g -fsanitize=address
