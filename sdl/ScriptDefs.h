#ifndef _SCRIPT_DEFS_
#define _SCRIPT_DEFS_

//Ordering: no args, 1 int, 1 address, 2 ints, 3 ints, 2 ints 1 address

const int oneintpos = 6;
const int oneaddresspos = 13;
const int twointpos = 16;
const int threeintpos = 22;
const int twointoneaddresspos = 24;

enum command{CMD_NOP, CMD_RETURN, CMD_HALT, CMD_UNHALT, CMD_FACEPLAYER, CMD_WAITFORCURRENTENTITY, 
	CMD_BRANCH, CMD_WAIT, CMD_SETTEXTBOXVISIBLE, CMD_SETTEXTBOXINVISIBLE, CMD_FACECURRENT, CMD_MOVECURRENT, CMD_WAITFORENTITY, 
	CMD_JUMP, CMD_JUMPSUBROUTINE, CMD_DISPLAY, 
	CMD_STORE, CMD_COPY, CMD_INCREMENT, CMD_DECREMENT, CMD_FACE, CMD_MOVE, 
	CMD_ADD, CMD_SUBTRACT, 
	CMD_CHOOSE};

#endif
