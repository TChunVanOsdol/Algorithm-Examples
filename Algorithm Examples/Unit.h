#pragma once
#include "Map.h"

class Unit {
public:
	Map * mapref;
	int position;
	bool enemyAgent = false;
	sf::CircleShape shape;

	bool selected = false;

	std::list<int> astarPath;

	Unit(Map & mref, bool isEnemy, int pos);
	void update(sf::RenderWindow & window);

private:
	void drawUnit(sf::RenderWindow & window);
	void selectUnit();
	void dragUnit(sf::RenderWindow & window);
	void moveUnit();
};