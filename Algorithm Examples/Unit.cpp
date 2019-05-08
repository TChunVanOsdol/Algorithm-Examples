#include "stdafx.h"
#include "Unit.h"

Unit::Unit(Map & mref, bool isEnemy, int pos) {
	mapref = &mref;
	shape.setRadius(mapref->tileW / 2.f);
	if (isEnemy) {
		shape.setFillColor(sf::Color::Red);
		enemyAgent = true;
	}
	else {
		shape.setFillColor(sf::Color::Blue);
	}
	position = pos;
}

void Unit::drawUnit(sf::RenderWindow & window) {
	shape.setPosition(mapref->getTilePos(position));
	window.draw(shape);
}