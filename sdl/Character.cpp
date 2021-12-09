#include <string>

#include "Character.h"

Character::Character(std::string n, int l, int mh, int mm, int a, int d, int s, bool ps){
	name = n;
	level = l;
	maxhp = mh;
	hp = mh;
	maxmp = mm;
	mp = mm;
	attack = a;
	defence = d;
	speed = s;
	playerside = ps;
	ko = false;
}

std::string Character::getName(){
	return name;
}

int Character::getLevel(){
	return level;
}

void Character::setLevel(int l){
	level = l;
}

int Character::getHP(){
	return hp;
}

void Character::setHP(int h){
	hp = h;
}

int Character::getMP(){
	return mp;
}

void Character::setMP(int m){
	mp = m;
}

int Character::getMaxHP(){
	return maxhp;
}

int Character::getMaxMP(){
	return maxmp;
}

int Character::getAttack(){
	return attack;
}

int Character::getDefence(){
	return defence;
}

int Character::getSpeed(){
	return speed;
}

bool Character::isKO(){
	return ko;
}

void Character::setKO(bool k){
	ko = k;
}

void Character::setTempIndex(int u){
	tempindex = u;
}

int Character::getTempIndex(){
	return tempindex;
}

void Character::setTempOnPlayerSide(bool u){
	temponplayerside = u;
}

bool Character::getTempOnPlayerSide(){
	return temponplayerside;
}
