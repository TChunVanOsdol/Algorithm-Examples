#pragma once
#include "stdafx.h"
#include "Map.h"

Map::Map(sf::RenderWindow & window, int tilesInRow, int tilesInCol) {
	mapSize = window.getSize();
	tilesPerRow = tilesInRow;
	tilesPerCol = tilesInCol;
	tileCount = tilesPerCol * tilesPerRow;
	tileW = mapSize.x / tilesPerRow;
	tileH = mapSize.y / tilesPerCol;
	tileShape.setSize({ tileW, tileH });
	tileShape.setOutlineThickness(1.f);
	tileShape.setOutlineColor(sf::Color::Yellow);
	previewUnitDrop.setRadius(tileW / 2.f);
	//Generate vector for tiles
	tiles = std::vector<tiletype>(tileCount);
	//Create border of walls
	for (int i = 0; i < tileCount; ++i) {
		if (i < tilesPerRow || i > tileCount - tilesPerRow || i % tilesPerRow == 0 || i % tilesPerRow == tilesPerRow - 1) {
			tiles[i] = wall;
		}
		else {
			tiles[i] = ground;
		}
	}
	//Fill walls anywhere you want here
	tiles[37] = wall;
	tiles[62] = wall;
	tiles[87] = wall;
	tiles[112] = wall;
	tiles[137] = wall;
	tiles[162] = wall;
	tiles[187] = wall;
	tiles[212] = wall;
	tiles[237] = wall;
	tiles[238] = wall;
	tiles[239] = wall;
	tiles[240] = wall;
}

void Map::drawMap(sf::RenderWindow & window) {
	//Move a single rectangle across the board to draw each tile
	sf::Vector2f tilePos;
	for (int i = 0; i < tileCount; i++) {
		tilePos = getTilePos(i);
		tileShape.setPosition(tilePos);
		if (tiles[i] == wall) {
			tileShape.setFillColor(sf::Color::White);
		}
		else {
			tileShape.setFillColor(sf::Color::Black);
		}
		window.draw(tileShape);
	}
}

int Map::getTileN(float x, float y) {
	//Find the index number starting at 0,0 going right then down
	int tileX, tileY;
	tileX = int(x / tileW);
	tileY = int(y / tileH);
	return tileX + (tileY * tilesPerCol);
}

sf::Vector2f Map::getTilePos(int N) {
	//Return position of tile's origin in pixels
	float posX, posY;
	posX = (N % tilesPerRow) * tileW;
	posY = (N / tilesPerCol) * tileH;
	return sf::Vector2f(posX, posY);
}

std::list<int> Map::astar(int start, int end) {
	//Use Map's tiles vector as nodes

	//Create containers that persist through each iteration

	//Find start node's cost and score

	//Find the lowest node by getting the lowest score in the open set

	//Finish algorithm if the current node is the end

	//Move current node from open set to closed set

	//Recalculate the set of neighbors

	//Add neighbor to open set if they do not already exist

	//Calculate heuristic and cost of neighbor

	//Update the best values of this neighbor (the previous node, cost, and score)

	//Replace with an incomplete list if desired
	std::list<int> emptyList;
	return emptyList;
}