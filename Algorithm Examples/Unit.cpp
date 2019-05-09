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

void Unit::update(sf::RenderWindow & window) {
	drawUnit(window);
	selectUnit();
	dragUnit(window);
	moveUnit();
}

void Unit::drawUnit(sf::RenderWindow & window) {
	shape.setPosition(mapref->getTilePos(position));
	window.draw(shape);
}

void Unit::selectUnit() {
	if (mapref->newClick && mapref->mousePos == position) {
		selected = true;
		if (enemyAgent) {
			mapref->previewUnitDrop.setFillColor(sf::Color(255, 0, 0, 127));
		}
		else {
			mapref->previewUnitDrop.setFillColor(sf::Color(0, 0, 255, 127));
		}
	}
}

void Unit::dragUnit(sf::RenderWindow & window) {
	if (mapref->clickDown && selected) {
		if (mapref->tiles[mapref->mousePos] != wall) {
			mapref->previewUnitDrop.setPosition(mapref->getTilePos(mapref->mousePos));
			window.draw(mapref->previewUnitDrop);
		}
	}
}

void Unit::moveUnit() {
	if (mapref->endClick && selected) {
		if (mapref->tiles[mapref->mousePos] != wall) {
			position = mapref->mousePos;
		}
		selected = false;
	}
}