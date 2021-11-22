Description of program arguments:

./main <program mode> <load from file> <map width/tileset length> <map height> <file name>


<program mode>: 0 = normal gameplay, 1 = map editor, 2 = tileset editor

<load from file>: 0 = create new map/tileset, 1 = load map/tileset from file

<map width/tileset length>: Integer. Width of new map or length of new tileset; ignored if <load from file> = 1

<map height>: Integer. Height of new map; ignored if <program mode> != 1 or <load from file> = 1

<file name>: String. Map/tileset file to save to, and to load from if <load from file> = 1


All arguments must be present for the program to run, even if they are not used. (Todo: implement a proper argument handler)
