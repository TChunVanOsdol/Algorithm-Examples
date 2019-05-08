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
	int nodeCount = tiles.size();
	int currentNode, neighborNode;
	sf::Vector2f startPos = getTilePos(start);
	sf::Vector2f endPos = getTilePos(end);
	//Create containers that persist through each iteration
	std::vector<int> priorNodes(nodeCount);
	std::set<int> openSet = { start };
	std::set<int> closedSet;
	//Add all walls to the closed set
	for (int i = 0; i < nodeCount; ++i) {
		if (tiles[i] == wall) {
			closedSet.insert(i);
		}
	}
	//Iterable list used to find neighboring nodes
	std::list<int> neighborNodes = {
		-tilesPerRow - 1,
		-tilesPerRow,
		-tilesPerRow + 1,
		-1,
		1,
		tilesPerRow - 1,
		tilesPerRow,
		tilesPerRow + 1
	};
	//Vectors of each node's cost and score
	std::vector<float> cost(nodeCount, INFINITY);
	std::vector<float> score(nodeCount, INFINITY);
	//Find start node's cost and score
	cost[start] = 0;
	score[start] = std::sqrtf(std::powf(startPos.x - endPos.x, 2) + std::powf(startPos.y - endPos.y, 2));
	while (!openSet.empty()) {
		//Find the lowest node by getting the lowest score in the open set
		float lowestScore = INFINITY;
		for (int node : openSet) {
			if (lowestScore > score[node]) {
				lowestScore = score[node];
				currentNode = node;
			}
		}
		//Get the (x,y) coordinates of the current node
		sf::Vector2f currentPos = getTilePos(currentNode);
		//Finish algorithm if the current node is the end
		if (currentNode == end) {
			//Assemble list of nodes from start to end
			std::list<int> fullPath = { currentNode };
			while (currentNode != start) {
				currentNode = priorNodes[currentNode];
				fullPath.push_front(currentNode);
			}
			return fullPath;
		}
		//Move current node from open set to closed set
		openSet.erase(currentNode);
		closedSet.insert(currentNode);
		//Recalculate the set of neighbors
		std::set<int> neighbors;
		for (int node : neighborNodes) {
			neighborNode = currentNode + node;
			//Skip this iteration if the neighbor is out-of-bounds
			if (neighborNode < 0 || neighborNode >= nodeCount) {
				continue;
			}
			//Use this neighbor if it is a pathable tile
			else if (tiles[neighborNode] == ground) {
				neighbors.insert(neighborNode);
			}
		}
		//Check each neigbhor
		for (int node : neighbors) {
			//Neighbor is in closed set, ignore this iteration
			if (closedSet.count(node) > 0) {
				continue;
			}
			//Add neighbor to open set if they do not already exist
			if (openSet.count(node) == 0) {
				openSet.insert(node);
			}
			//Calculate heuristic and cost of neighbor
			sf::Vector2f neighborPos = getTilePos(node);
			float heuristic = std::sqrtf(std::powf(neighborPos.x - endPos.x, 2) + std::powf(neighborPos.y - endPos.y, 2));
			float dist = std::sqrtf(std::powf(neighborPos.x - currentPos.x, 2) + std::powf(neighborPos.y - currentPos.y, 2));
			float tempCost = cost[currentNode] + dist;
			//Ignore this neighbor if it costs more to get there than from another path
			if (tempCost >= cost[node]) {
				continue;
			}
			//Update the best values of this neighbor (the previous node, cost, and score)
			priorNodes[node] = currentNode;
			cost[node] = tempCost;
			score[node] = tempCost + heuristic;
		}
	}
	//Error! Algorithm ended early (there was probably no path to target)
	std::list<int> errorList = {start};
	return errorList;
}