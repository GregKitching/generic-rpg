CPP = g++
CFLAGS = -03
LDFLAGS =
DEPS = constants.h enums.h SpriteSheet.h Tile.h TileSet.h Map.h Entity.h ActiveEntity.h TextBox.h SpeechTextBox.h MenuTextBox.h BookTextBox.h StaticTextBox.h Script.h SoundHandler.h Character.h BattleChoice.h BattleManager.h
OBJ = main.o SpriteSheet.o Tile.o TileSet.o Map.o Entity.o ActiveEntity.o TextBox.o SpeechTextBox.o MenuTextBox.o BookTextBox.o StaticTextBox.o Script.o SoundHandler.o Character.o BattleManager.o

sdl_cflags := $(shell pkg-config --cflags sdl2 SDL2_mixer SDL2_image)
sdl_libs := $(shell pkg-config --libs sdl2 SDL2_mixer SDL2_image)
override CFLAGS += $(sdl_cflags)
override LIBS += $(sdl_libs)

%.o: %.cpp $(DEPS)
	$(CPP) $(LDFLAGS) -g -fsanitize=address -c -o $@ $< $(LIBS)

main: $(OBJ)
	$(CPP) $(LDFLAGS) -g -fsanitize=address -o $@ $^ $(LIBS)

clean:
	rm -f main *.o
.PHONY: all clean
#-g -fsanitize=address
