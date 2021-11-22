Description of program arguments:

./main <program mode> <load from file> <map width/tileset length> <map height> <file name>


<program mode>: 0 = normal gameplay, 1 = map editor, 2 = tileset editor

<load from file>: 0 = create new map/tileset, 1 = load map/tileset from file

<map width/tileset length>: Integer. Width of new map or length of new tileset; ignored if <load from file> = 1

<map height>: Integer. Height of new map; ignored if <program mode> != 1 or <load from file> = 1

<file name>: String. Map/tileset file to save to, and to load from if <load from file> = 1


All arguments must be present for the program to run, even if they are not used. (Todo: implement a proper argument handler)



Program Components:

Main
The main loop of the program spawns two other threads which handle specific tasks.
	RenderFunc is continuously called 60 times per second using SDL_AddTimer. It renders everything to the display and sets the
	pace for everything that needs to be done at regular intervals, such as the speed at which the player walks or text in
	speech-type text boxes crawls accross the screen.
	
	scriptThread enters a while loop and executes commands from the currently active script independantly of rendering, calling
	usleep() if the script must wait for an action to be performed or there is no currently active script.

The structure of the program, including the way maps, entities and scripts work, were inspired by the the 3rd generation of Pokemon
games by Game Freak, and the way in which that data was displayed to us outsiders by third-party rom editing tools such as AdvanceMap.

Map
Maps are made up of an x by y grid of 16 by 16 pixel tiles, a movement permission for each tile, an "outside tile" to render in visible
areas outside the map, and a set of entities to spawn within the map.

Tileset
Each tile in the tileset is actually eight references to 8 by 8 pixel subtiles taken from a character set generated from
basictilesmodified.png in the images folder. A 4 by 4 grid of subtiles is rendered, and then another 4 by 4 grid is rendered on top of
it, with parts of the first four subtiles potentially showing through. This allows a large number of tiles to be constructed from a
much smaller number of subtiles, as certain patterns can be repeated on multiple tiles without much redundancy in video memory. It also
potentially allows entities to be rendered in between the two layers, creating the illusion of depth in the world, though this has not
currently been implemented.

Movement Permissions
Dictates how entities can move. There are currently four types:
	MOVEMENT_DEFAULT: Entities can always walk onto this layer. Serves as a sort of bridge between the two main walking layers,
	MOVEMENT_LAYER0 and MOVEMENT_LAYER1.
	
	MOVEMENT_BLOCKED: Entities can never move onto this tile.
	
	MOVEMENT_LAYER0 and MOVEMENT_LAYER1: Entities that are currently on one of these layers can move onto an adjacent tile of the 
	same layer or MOVEMENT_DEFAULT, but not the other layer. This allows normally walkable tiles to be blocked from a certain 
	direction, such as the ground to the north of cliffs, which are technically adjacent to the cliff and would be walkable to 
	otherwise.

Entities
Entities are placed around maps as objects for the player to interact with. They can be visible, physical things like people, 
triggering a script when the player presses the action button while facing them, or they can be invisible and trigger a script when
the player walks on them.
Technically the player is also an entity, but is handled differently from other entities. The player is always entity number zero, has
no script associated with them, and does not despawn when the map changes.

Scripts
Scripts allow things to happen in the game, such as entities moving, text boxes spawning, warping the player to another map, and 
eventually adding items to the inventory and starting battles. Scripts can be launced from other scripts, and can take branching paths
depending on the player's choices.
The command set is documented in assets/scripts/command_set.txt. Scripts written as text files must be compiled into binary files
using the program in assets/scripts, and must have a name consisting of four lowercase letters followed by .scr. Maps and Text files are also reffered to in this way. Todo: make a more general approach to referencing files from within scripts.

Text Boxes
Display text on the screen for the user to read. There are currently three main types: speech types, where the text slowly crawls
in and user input is required to advance a line; choice types, where the user must select one of the lines to continue, with the
possibility that there are more choices than can be displayed at once and the user can scroll through them; and book types, where a
number of lines are displayed at once and the user can flip through the pages of text. Text Boxes were designed to be robust, allowing
multiple types to use the same code.
