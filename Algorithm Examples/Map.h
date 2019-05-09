#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <set>

enum tiletype {
	ground,
	wall
};

class Map {
public:
	sf::RectangleShape tileShape;
	sf::Vector2u mapSize;
	float tileH, tileW;
	int tilesPerRow;
	int tilesPerCol;
	int tileCount;
	std::vector<tiletype> tiles;

	int mousePos;
	bool newClick = false;
	bool endClick = false;
	bool clickDown = false;

	sf::CircleShape previewUnitDrop;

	Map(sf::RenderWindow & window, int tilesInRow, int tilesInCol);
	void drawMap(sf::RenderWindow & window);
	int getTileN(float x, float y);
	sf::Vector2f getTilePos(int N);

	std::list<int> astar(int start, int end);
};