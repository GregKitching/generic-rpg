#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include <unistd.h>

#include "Globals.h"
#include "enums.h"
#include "BattleChoice.h"
#include "BattleManager.h"

BattleManager::BattleManager(std::vector<Character*> p, std::vector<Character*> e){
	//activephase = false;
	inbattle = true;
	//selectphase = false;
	blocked = false;
	back = false;
	characteracting = 0;
	//choice = 0;
	partysize = p.size();
	enemysize = e.size();
	//actiontextbox = NULL;
	//chooseenemytextbox = NULL;
	//partyactions = new battleaction[p->size()];
	partytextboxlines.push_back("      ");//character name
	partytextboxlines.push_back("Lv.    ");
	partytextboxlines.push_back("HP:    /   ");
	partytextboxlines.push_back("MP:    /   ");
	//enemystartindex = p->size();
	for(int i = 0; i < p.size(); i++){
		//party->at(i)->
		//party->at(i)->setTempOnPlayerSide(true);
		characters.push_back(p.at(i));
		characters.at(i)->setTempIndex(i);
		characters.at(i)->setTempOnPlayerSide(true);
	}
	for(int i = 0; i < e.size(); i++){
		characters.push_back(e.at(i));
		characters.at(i + partysize)->setTempIndex(i + partysize);
		characters.at(i + partysize)->setTempOnPlayerSide(false);
		//enemies->at(i)->setTempOnPlayerSide(false);
		enemytextboxlines.push_back("      :    ");
		//enemytextboxlines.push_back("HP:    ");
	}
	menutextboxlines.push_back("Attack");
	menutextboxlines.push_back("Magic");
	menutextboxlines.push_back("Escape");
	introtextboxline.push_back("Enemies have appeared!");
	//notenoughmpline.push_back("Not enough MP");
	//bc = new BattleChoice[partysize + enemysize];
	printf("a\n");
	printf("u %ld\n", enemytextboxlines.size());
}

BattleManager::~BattleManager(){
	std::vector<Character*> u = {};
	characters.clear();
	characters.swap(u);//This will actually free the memory associated with the vector
	std::vector<std::string> u2 = {};
	partytextboxlines.clear();
	partytextboxlines.swap(u2);
	enemytextboxlines.clear();
	enemytextboxlines.swap(u2);
	menutextboxlines.clear();
	menutextboxlines.swap(u2);
	introtextboxline.clear();
	introtextboxline.swap(u2);
	//notenoughmpline.clear();
	//notenoughmpline.swap(u2);
	std::vector<BattleChoice*> u3 = {};
	bc.clear();
	bc.swap(u3);
}

bool BattleManager::sortFunction(BattleChoice *a, BattleChoice *b){
	return a->source->getSpeed() > b->source->getSpeed();
}

void BattleManager::createActionTextBox(){
	//actionTextBox = 
	textboxes.push_back(new MenuTextBox(184, 0, 72, 64, 16, 8, 56, 56));
	textboxes.at(textboxes.size() - 1)->loadText(menutextboxlines);
	//textboxes.push_back(actionTextBox);
}

void BattleManager::createEnemyTextBox(){
	textboxes.push_back(new MenuTextBox(0, 0, 112, (enemysize * 16) + 16, 12, 8, 96, enemysize * 16));
	textboxes.at(textboxes.size() - 1)->loadText(enemytextboxlines);
	textboxes.at(textboxes.size() - 1)->setText(textboxes.at(partysize)->getLines());
}

void BattleManager::createMagicTextBox(){
	textboxes.push_back(new MenuTextBox(0, 32, 160, 32, 16, 8, 144, 24));
	std::vector<std::string> s;
	s.push_back("Magic Missile (5)");
	textboxes.at(textboxes.size() - 1)->loadText(s);
}

void BattleManager::createMessageTextBox(std::string s){
	textboxes.push_back(new SpeechTextBox(0, 50, (s.size() * 8) + 16, 32, 8, 8, s.size() * 8, 16, 2));
	std::vector<std::string> s2;
	s2.push_back(s);
	textboxes.at(textboxes.size() - 1)->loadText(s2);
}

void BattleManager::waitForInput(){
	blocked = true;
	printf("Blocked\n");
	while(blocked){
		usleep(16666);
	}
}

void BattleManager::destroyTextBox(){
	textboxes.at(textboxes.size() - 1)->close();
	if(textboxes.at(textboxes.size() - 1)->getType() == TEXTBOX_SPEECH){
		delete static_cast<SpeechTextBox*>(textboxes.at(textboxes.size() - 1));
	} else if(textboxes.at(textboxes.size() - 1)->getType() == TEXTBOX_MENU){
		delete static_cast<MenuTextBox*>(textboxes.at(textboxes.size() - 1));
	} else if(textboxes.at(textboxes.size() - 1)->getType() == TEXTBOX_BOOK){
		delete static_cast<BookTextBox*>(textboxes.at(textboxes.size() - 1));
	} else if(textboxes.at(textboxes.size() - 1)->getType() == TEXTBOX_STATIC){
		delete static_cast<StaticTextBox*>(textboxes.at(textboxes.size() - 1));
	}
	textboxes.pop_back();
}

void BattleManager::attack(Character *source, Character *target){
	int damage = source->getAttack() - target->getDefence();
	if(damage < 0){
		damage = 0;
	}
	int hp = target->getHP() - damage;
	if(hp <= 0){
		hp = 0;
		target->setKO(true);
		printf("KO\n");
	}
	target->setHP(hp);
	if(target->getTempOnPlayerSide()){
		setPartyTextHP(target->getTempIndex(), hp);
	} else {
		setEnemyTextHP(target->getTempIndex() - partysize, hp);
	}
	std::string s = "Did " + std::to_string(damage) + " damage!";
	createMessageTextBox(s);
	waitForInput();
	destroyTextBox();
}

void BattleManager::magic(Character *source, Character *target, int itemindex){
	int damage = source->getAttack() - target->getDefence();
	int mp = source->getMP() - 5;//Placeholder value until spells are defined
	if(damage < 0){
		damage = 0;
	}
	int hp = target->getHP() - damage;
	if(hp <= 0){
		hp = 0;
		target->setKO(true);
	}
	target->setHP(hp);
	source->setMP(mp);
	if(target->getTempOnPlayerSide()){
		setPartyTextHP(target->getTempIndex(), hp);
	} else {
		setEnemyTextHP(target->getTempIndex() - partysize, hp);
	}
	setPartyTextMP(source->getTempIndex(), mp);
	std::string s = "Did " + std::to_string(damage) + " damage!";
	createMessageTextBox(s);
	waitForInput();
	destroyTextBox();
}

void BattleManager::setPartyTextName(int num, std::string s){
	textboxes.at(num)->changeText(0, 0, s.size(), s);
}

void BattleManager::setPartyTextLevel(int num, int l){
	std::string s = std::to_string(l);
	textboxes.at(num)->changeText(1, 4, s.size() + 4, s);
}

void BattleManager::setPartyTextHP(int num, int h){
	std::string s = std::to_string(h);
	if(h <  10){
		s.insert(0, " ");
	}
	if(h < 100){
		s.insert(0, " ");
	}
	textboxes.at(num)->changeText(2, 4, -1, s);
}

void BattleManager::setPartyTextMaxHP(int num, int mh){
	std::string s = std::to_string(mh);
	textboxes.at(num)->changeText(2, 9, s.size() + 9, s);
}

void BattleManager::setPartyTextMP(int num, int m){
	std::string s = std::to_string(m);
	if(m <  10){
		s.insert(0, " ");
	}
	if(m < 100){
		s.insert(0, " ");
	}
	textboxes.at(num)->changeText(3, 4, -1, s);
}

void BattleManager::setPartyTextMaxMP(int num, int mm){
	std::string s = std::to_string(mm);
	textboxes.at(num)->changeText(3, 9, s.size() + 9, s);
}

void BattleManager::setEnemyTextName(int num, std::string s){
	textboxes.at(partysize)->changeText(num, 0, s.size(), s);
}

void BattleManager::setEnemyTextHP(int num, int h){
	std::string s = std::to_string(h);
	if(s.size() == 1){
		s.append("  ");
	} else if (s.size() == 2){
		s.append(" ");
	}
	textboxes.at(partysize)->changeText(num, 8, 11, s);
}

void BattleManager::buttonAction(){
	back = false;
	blocked = false;
	/*textbox *t = textboxes.at(textboxes.size() - 1);
	if(t->isActive()){
		if(t->getType() == TEXTBOX_SPEECH){
			t->next();
			if(t->isActive()){
				textboxes.pop_back();
			}
		} else {
			choice = t->close();
		}
	}*/
}

void BattleManager::backButtonAction(){
	back = true;
	blocked = false;
	/*if(!activephase && (characteracting != 0 || selectphase)){
		int f = textboxes.at(textboxes.size() - 1)->close();
		if(selectphase){
			selectphase = false;
			delete textboxes.at(textboxes.size() - 1);
			textboxes.pop_back();
		} else {
			characteracting--;
		}
	}*/
}

void BattleManager::dirAction(dir direction){
	TextBox *t = textboxes.at(textboxes.size() - 1);
	if(t->getType() == TEXTBOX_MENU){
		if(direction == DIR_UP){
			t->prev();
		} else if (direction == DIR_DOWN){
			t->next();
		}
	}
}

void BattleManager::run(){
	if(!textboxes.empty()){
		textboxes.erase(textboxes.begin(), textboxes.end());
	}
	StaticTextBox *t;
	for(int i = 0; i < partysize; i++){
		t = new StaticTextBox(96 * i, 104, 104, 88, 8, 8, 80, 72);
		t->loadText(partytextboxlines);
		textboxes.push_back(t);
		setPartyTextName(i, characters.at(i)->getName());
		setPartyTextLevel(i, characters.at(i)->getLevel());//Attack
		setPartyTextHP(i, characters.at(i)->getHP());	//Defend
		setPartyTextMaxHP(i, characters.at(i)->getMaxHP());//Magic
		setPartyTextMP(i, characters.at(i)->getMP());	//Escape
		setPartyTextMaxMP(i, characters.at(i)->getMaxMP());
	}
	t = new StaticTextBox(0, 0, 112, (enemysize * 16) + 16, 12, 8, 96, enemysize * 16);
	t->loadText(enemytextboxlines);
	textboxes.push_back(t);
	for(int i = 0; i < enemysize; i++){
		setEnemyTextName(i, characters.at(i + partysize)->getName());
		setEnemyTextHP(i, characters.at(i + partysize)->getHP());
	}
	textboxes.push_back(new SpeechTextBox(4, 123, 248, 56, 4, 4, 240, 48, 2));
	textboxes.at(textboxes.size() - 1)->loadText(introtextboxline);
	blocked = true;
	while(blocked){//textboxes.at(textboxes.size() - 1)->isActive()){
		usleep(16666);
	}
	destroyTextBox();
	
	
	
	TURNSTART: characteracting = 0;
	bc.erase(bc.begin(), bc.end());
	for(int i = 0; i < characters.size(); i++){
		bc.push_back(new BattleChoice());
		bc.at(i)->source = characters.at(i);
	}
	
	CREATEACTIONTEXTBOX: createActionTextBox();
	
	WAITFORACTIONINPUT: waitForInput();
	if(back){
		if(characteracting > 0){
			characteracting--;
			destroyTextBox();
			goto CREATEACTIONTEXTBOX;//JESUS CHRIST HOW HORRIFYING
		} else {
			goto WAITFORACTIONINPUT;
		}
	} else {
		choice = (battleaction) textboxes.at(textboxes.size() - 1)->getValue();
		bc.at(characteracting)->action = choice;
		if(choice == BATTLEACTION_ATTACK){
			goto CREATEENEMYTEXTBOXATTACK;
		} else if (choice == BATTLEACTION_MAGIC){
			goto CREATEMAGICTEXTBOX;
		} else {//if (choice == BATTLEACTION_ESCAPE){
			goto ESCAPEACTION;
		}
	}
	
	CREATEENEMYTEXTBOXATTACK: createEnemyTextBox();
	waitForInput();
	printf("%d\n", characteracting);
	if(back){	
		destroyTextBox();
		goto WAITFORACTIONINPUT;
	} else {
		bc.at(characteracting)->target = characters.at(textboxes.at(textboxes.size() - 1)->getValue() + partysize);
		bc.at(characteracting)->itemindex = 0;
		destroyTextBox();
		destroyTextBox();
		if(characteracting == partysize - 1){
			goto BATTLEPHASE;
		} else {
			characteracting++;
			goto CREATEACTIONTEXTBOX;
		}
	}
	
	CREATEMAGICTEXTBOX: createMagicTextBox();
	
	WAITFORMAGICINPUT: waitForInput();
	if(back){
		destroyTextBox();
		goto WAITFORACTIONINPUT;
	} else {
		if(characters.at(characteracting)->getMP() < 5){
			createMessageTextBox("Not enough MP");
			waitForInput();
			destroyTextBox();
			goto WAITFORMAGICINPUT;
		} else {
			bc.at(characteracting)->itemindex = textboxes.at(textboxes.size() - 1)->getValue();
			goto CREATEENEMYTEXTBOXMAGIC;
		}
	}
	
	CREATEENEMYTEXTBOXMAGIC: createEnemyTextBox();
	waitForInput();
	if(back){	
		destroyTextBox();
		goto WAITFORMAGICINPUT;
	} else {
		bc.at(characteracting)->target = characters.at(textboxes.at(textboxes.size() - 1)->getValue() + partysize);
		destroyTextBox();
		destroyTextBox();
		destroyTextBox();
		if(characteracting == partysize - 1){
			goto BATTLEPHASE;
		} else {
			characteracting++;
			goto CREATEACTIONTEXTBOX;
		}
	}
	
	ESCAPEACTION: destroyTextBox();
	bc.at(characteracting)->target = NULL;
	bc.at(characteracting)->itemindex = 0;
	if(characteracting == partysize - 1){
		goto BATTLEPHASE;
	} else {
		characteracting++;
		goto CREATEACTIONTEXTBOX;
	}
	
	BATTLEPHASE: for(int i = partysize; i < partysize + enemysize; i++){
		bc.at(i)->action = BATTLEACTION_ATTACK;
		bc.at(i)->target = characters.at(0);
		bc.at(i)->itemindex = 0;
	}
	std::sort(bc.begin(), bc.end(), sortFunction);
	for(int i = 0; i < characters.size(); i++){
		if(!(bc.at(i)->source->isKO())){
			if(bc.at(i)->action == BATTLEACTION_ATTACK){
				createMessageTextBox(bc.at(i)->source->getName() + " attacks!");
				waitForInput();
				destroyTextBox();
				if(bc.at(i)->target->isKO()){
					createMessageTextBox(bc.at(i)->target->getName() + " is already dead!");
					waitForInput();
					destroyTextBox();
				} else {
					attack(bc.at(i)->source, bc.at(i)->target);
					if(bc.at(i)->target->getHP() == 0){
						createMessageTextBox(bc.at(i)->target->getName() + " died!");
						waitForInput();
						destroyTextBox();
					}
				}
			} else if (bc.at(i)->action == BATTLEACTION_MAGIC){
				createMessageTextBox(bc.at(i)->source->getName() + " uses magic!");
				waitForInput();
				destroyTextBox();
				if(bc.at(i)->target->isKO()){
					createMessageTextBox(bc.at(i)->target->getName() + " is already dead!");
					waitForInput();
					destroyTextBox();
				} else {
					magic(bc.at(i)->source, bc.at(i)->target, 0);
					if(bc.at(i)->target->getHP() == 0){
						createMessageTextBox(bc.at(i)->target->getName() + " died!");
						waitForInput();
						destroyTextBox();
					}
				}
			} else {
				goto ESCAPE;
			}
			bool ko = true;
			for(int i = 0; i < partysize; i++){
				ko = ko & characters.at(i)->isKO();
			}
			if(ko){
				goto LOSE;
			}
			ko = true;
			for(int i = partysize; i < partysize + enemysize; i++){
				ko = ko & characters.at(i)->isKO();
			}
			if(ko){
				goto WIN;
			}
		}
	}
	goto TURNSTART;
	
	ESCAPE: createMessageTextBox("Escaped!");
	waitForInput();
	destroyTextBox();
	goto LEAVE;
	
	LOSE: createMessageTextBox("Everyone's dead!");
	waitForInput();
	destroyTextBox();
	goto LEAVE;
	
	WIN: createMessageTextBox("You win!");
	waitForInput();
	destroyTextBox();
	goto LEAVE;
	
	LEAVE: destroyTextBox();
	for(int i = 0; i < partysize; i++){
		destroyTextBox();
	}
}
