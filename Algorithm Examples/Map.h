#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <random>
#include <iostream>

template <typename T>
bool operator > (const sf::Vector2<T> & lhs, const sf::Vector2<T> & rhs) { return (lhs.x > rhs.x && lhs.y > rhs.y); }

template <typename T>
bool operator < (const sf::Vector2<T> & lhs, const sf::Vector2<T> & rhs) { return rhs > lhs; }

template <typename T>
bool operator >= (const sf::Vector2<T> & lhs, const sf::Vector2<T> & rhs) { return !(rhs > lhs); }

template <typename T>
bool operator <= (const sf::Vector2<T> & lhs, const sf::Vector2<T> & rhs) { return !(lhs > rhs); }

enum tiletype {
	ground,
	wall,
	water
};

struct room {
	sf::Vector2i position = { 0,0 };
	sf::Vector2i size = { 0,0 };
	std::set<room*> connectedRooms = { this };
	std::list<sf::Vector2i> doorList;
	room() {}
	room(sf::Vector2i pos, sf::Vector2i sz) : position(pos), size(sz) {}
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

	std::minstd_rand rng;
	int currentSeed = 1;


	std::list<room*> rooms;

	Map(sf::RenderWindow & window, int tilesInRow, int tilesInCol);
	~Map();
	void drawMap(sf::RenderWindow & window);
	int getTileN(float x, float y);
	int intXYtoN(int x, int y); //{ return x + y * tilesPerRow; }
	sf::Vector2f getTilePos(int N);

	std::list<int> astar(int start, int end);

	void generateMap(int seed);
	void hallsByPairs();
	void hallsWeightedProbs();
};