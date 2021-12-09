#ifndef _CHARACTER_
#define _CHARACTER_

class Character{
	private:
		int level, hp, maxhp, mp, maxmp, attack, defence, speed, tempindex;
		bool playerside, ko, temponplayerside;
		std::string name;
	public:
		Character(std::string n, int l, int mh, int mm, int a, int d, int s, bool ps);
		std::string getName();
		int getLevel();
		void setLevel(int l);
		int getHP();
		void setHP(int h);
		int getMP();
		void setMP(int m);
		int getMaxHP();
		int getMaxMP();
		int getAttack();
		int getDefence();
		int getSpeed();
		bool isKO();
		void setKO(bool k);
		void setTempIndex(int u);
		int getTempIndex();
		void setTempOnPlayerSide(bool u);
		bool getTempOnPlayerSide();
};

#endif
