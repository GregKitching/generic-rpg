#ifndef _BATTLE_CHOICE_
#define _BATTLE_CHOICE_

struct BattleChoice{
	Character *source;
	Character *target;
	battleaction action;
	int itemindex;
};

#endif
