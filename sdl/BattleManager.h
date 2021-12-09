#ifndef _BATTLE_MANAGER_
#define _BATTLE_MANAGER_

class BattleManager{
	private:
		std::vector<Character*> characters;
		//std::vector<Character*> *enemies;
		std::vector<std::string> partytextboxlines;
		std::vector<std::string> enemytextboxlines;
		std::vector<std::string> menutextboxlines;
		std::vector<std::string> introtextboxline;
		//std::vector<std::string> notenoughmpline;
		//TextBox *actiontextbox;
		//TextBox *enemyselecttextbox;
		bool activephase, inbattle, selectphase, blocked, back;
		int partysize, enemysize, characteracting, turn;
		battleaction choice;
		std::vector<BattleChoice*> bc;
		static bool sortFunction(BattleChoice *a, BattleChoice *b);
		void createActionTextBox();
		void createMagicTextBox();
		void createEnemyTextBox();
		void createMessageTextBox(std::string s);
		void waitForInput();
		void destroyTextBox();
		void attack(Character *source, Character *target);
		void magic(Character *source, Character *target, int itemindex);
	public:
		BattleManager(std::vector<Character*> p, std::vector<Character*> e);
		~BattleManager();
		void setPartyTextName(int num, std::string s);
		void setPartyTextLevel(int num, int l);
		void setPartyTextHP(int num, int h);
		void setPartyTextMaxHP(int num, int mh);
		void setPartyTextMP(int num, int m);
		void setPartyTextMaxMP(int num, int mm);
		void setEnemyTextName(int num, std::string s);
		void setEnemyTextHP(int num, int h);
		void buttonAction();
		void backButtonAction();
		void dirAction(dir direction);
		void run();
};

#endif
