CPP = g++
CFLAGS =
DEPS =
OBJ = main.o

%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CPP) -o $@ $^ $(CFLAGS)

clean:
	rm -f main *.o
.PHONY: all clean
