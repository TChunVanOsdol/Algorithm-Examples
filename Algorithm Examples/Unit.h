#pragma once
#include "Map.h"

class Unit {
public:
	Map * mapref;
	int position;
	bool enemyAgent = false;
	sf::CircleShape shape;

	std::list<int> astarPath;

	Unit(Map & mref, bool isEnemy, int pos);
	void drawUnit(sf::RenderWindow & window);

};