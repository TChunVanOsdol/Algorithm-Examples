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

Map::~Map() {
	for (std::list<room*>::reverse_iterator it = rooms.rbegin(); it != rooms.rend(); ++it)
		delete *it;
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
		else if (tiles[i] == ground) {
			tileShape.setFillColor(sf::Color::Black);
		}
		else {
			tileShape.setFillColor(sf::Color::Blue);
		}
		window.draw(tileShape);
	}
}

int Map::getTileN(float x, float y) {
	//Find the index number starting at 0,0 going right then down
	int tileX, tileY;
	tileX = int(x / tileW);
	tileY = int(y / tileH);
	return tileX + (tileY * tilesPerRow);
}

int Map::intXYtoN(int x, int y) {
	if (x < 0 || y < 0 || x >= tilesPerRow || y >= tilesPerCol) {
		return -1;
	}
	return x + y * tilesPerRow;;
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

void Map::generateMap(int seed) {
	//Generate a map, and use a new seed if provided

	//Create constants to constrain random number generator
	const int minSize = 2;
	const int maxSize = 6;
	const int sizeMod = maxSize - minSize + 1; //Add one to include max value
	const int roomDist = 3;
	const sf::Vector2i posMin = { 1,1 };
	const sf::Vector2i posMod = { tilesPerRow - minSize - 1, tilesPerCol - minSize - 1 };
	//Reset random number generator if seed changes
	if (seed != currentSeed) {
		rng.seed(seed);
		currentSeed = seed;
	}
	//Reset map to all wall tiles
	tiles = std::vector<tiletype>(tileCount, wall);
	for (std::list<room*>::reverse_iterator it = rooms.rbegin(); it != rooms.rend(); ++it)
		delete *it;
	rooms.clear();
	//Create a set of all tiles that cannot have a new room appear
	std::set<int> noRoomSpawn;
	std::set<int> tempRoom;
	//Create vectors for new room location and size
	sf::Vector2i roomPos;
	sf::Vector2i roomSize;
	int failedRooms = 0;
	//Count rooms to add halls later
	//Generate rooms until enough failures happened. There's a lot of room for creative cutoffs!
	while (failedRooms < 15) {
		//Reset values
		bool roomFailed = false;
		tempRoom.clear();
		//Pick a new random location for a room
		roomPos.x = rng() % posMod.x + posMin.x;
		roomPos.y = rng() % posMod.y + posMin.y;
		//Check if this room is valid by its width
		roomSize.x = rng() % sizeMod + minSize;
		while (roomPos.x + roomSize.x > tilesPerRow - 1) {
			--roomSize.x;
			if (roomSize.x < minSize) {
				roomFailed = true;
				++failedRooms;
				break;
			}
		}
		if (roomFailed)
			continue;
		//Check if this room is valid by its height
		roomSize.y = rng() % sizeMod + minSize;
		while (roomPos.y + roomSize.y > tilesPerCol - 1) {
			--roomSize.y;
			if (roomSize.y < minSize) {
				roomFailed = true;
				++failedRooms;
				break;
			}
		}
		if (roomFailed)
			continue;
		//Iterate through all potential new room tiles
		for (int y = roomPos.y; y < roomPos.y + roomSize.y; ++y) {
			for (int x = roomPos.x; x < roomPos.x + roomSize.x; ++x) {
				if (noRoomSpawn.count(intXYtoN(x, y)) == 0) {
					//Tile is valid, add to potential new room
					tempRoom.insert(intXYtoN(x, y));
				}
				else {
					//Tile invalidates this room, room is failed
					roomFailed = true;
					++failedRooms;
					break;
				}
			}
			if (roomFailed) {
				//If the room failed, no reason to continue iterating through y
				break;
			}
		}
		//Create room if it wasn't invalidated
		if (!roomFailed) {
			rooms.push_back(new room(roomPos, roomSize));
			for (int tile : tempRoom) {
				tiles[tile] = ground;
			}
			//Iterate through all affected tiles of the new room
			for (int y = roomPos.y - roomDist; y < roomPos.y + roomSize.y + roomDist; ++y) {
				for (int x = roomPos.x - roomDist; x < roomPos.x + roomSize.x + roomDist; ++x) {
					//Prevent all tiles within distance from becoming a new room
					noRoomSpawn.insert(intXYtoN(x, y));
				}
			}
		}
	}
	//hallsByPairs();
	hallsWeightedProbs();
}

void Map::hallsByPairs() {
	//Create halls by finding a pair of doors and drawing a hallway shape based on the doors' facings
		//Approach:
		//Pick two doors of two separate rooms
		//Face doors towards each other using weighted probabilities based on distance (longer distance is higher chance)
		//Extend halls from doors using its direction. For 3-link halls, pick a random x or y that both links have

	//Convenience parameters
	typedef std::pair<sf::Vector2i, room*> doorType;
	enum dir {
		east,
		west,
		south,
		north
	};
	sf::Vector2i paths[4] = {
		sf::Vector2i(1,0),
		sf::Vector2i(-1,0),
		sf::Vector2i(0,1),
		sf::Vector2i(0,-1)
	};
	//Algorithm parameters
	std::vector<doorType> doors; //Use a vector to make accessing random elements easier
	const int borderBuffer = 1; //The space between room and outer border needs to be at least 1
	//Start algorithm
	if (!rooms.empty()) {
		int roomCount = rooms.size();
		int connectedCount = 1;
		//Determine door locations
		for (room * r : rooms) {
			//Find top and bottom doors
			for (int y = r->position.y; y < r->position.y + r->size.y; y += r->size.y - 1) {
				for (int x = r->position.x; x < r->position.x + r->size.x; ++x) {
					//Add door if location is valid
					if (x > borderBuffer && y > borderBuffer && x < tilesPerRow - borderBuffer - 1 && y < tilesPerCol - borderBuffer - 1) {
						doorType newDoor = { sf::Vector2i(x,y), r };
						doors.push_back(newDoor);
					}
				}
			}
			//Find left and right doors (don't include tiles from top and bottom doors)
			for (int x = r->position.x; x < r->position.x + r->size.x; x += r->size.x - 1) {
				for (int y = r->position.y + 1; y < r->position.y + r->size.y - 1; ++y) {
					//Add door if location is valid
					if (x > borderBuffer && y > borderBuffer && x < tilesPerRow - borderBuffer - 1 && y < tilesPerCol - borderBuffer - 1) {
						doorType newDoor = { sf::Vector2i(x,y), r };
						doors.push_back(newDoor);
					}
				}
			}
		}
		int doorCount;

		doorType currentDoor;
		sf::Vector2i currentPos;
		room * currentRoom;
		dir currentDir;  //Direction: 0 +x, 1 -x, 2 +y, 3 -y

		doorType targetDoor;
		sf::Vector2i targetPos;
		room * targetRoom;
		dir targetDir;  //Direction: 0 +x, 1 -x, 2 +y, 3 -y

		std::set<int> tempHall; //Use a set since only the integer values are used for setting ground tiles

		//Iterate until all rooms are connected
		while (connectedCount < roomCount) {
			//Choose a random door
			doorCount = doors.size();
			//If there are no more doors, end the algorithm
			if (doorCount == 0)
				break;
			int doorID = rng() % doorCount;
			currentDoor = doors[doorID];
			currentPos = currentDoor.first;
			currentRoom = currentDoor.second;
			doors.erase(doors.begin() + doorID);
			//Choose another random door
			bool checkingDoor = true;
			//Use a copy of doors to eliminate doors from same room without removing from main vector
			std::vector<doorType> testDoors = doors;
			while (testDoors.size() > 0 && checkingDoor) {
				doorCount = testDoors.size();
				doorID = rng() % doorCount;
				targetDoor = testDoors[doorID];
				targetPos = targetDoor.first;
				targetRoom = targetDoor.second;
				//Check if this door is from the same room
				if (currentRoom == targetRoom) {
					testDoors.erase(testDoors.begin() + doorID);
					continue;
				}
				//End loop and remove door from main vector
				checkingDoor = false;
				doors.erase(std::find(doors.begin(), doors.end(), targetDoor));
			}
			//Clear out hallway set
			tempHall.clear();
			//Find directions of doors and build halls
			sf::Vector2i doorDist = targetPos - currentPos;
			sf::Vector2i tempPos = currentPos;
			//Doors are aligned vertically
			if (doorDist.x == 0) {
				if (doorDist.y > 0) {
					currentDir = south;
				}
				else {
					currentDir = north;
				}
				//Build hall
				while (tempPos != targetPos) {
					tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
					tempPos += paths[currentDir];
				}
			}
			//Doors are aligned horizontally
			else if (doorDist.y == 0) {
				if (doorDist.x > 0) {
					currentDir = east;
				}
				else {
					currentDir = west;
				}
				//Build hall
				while (tempPos != targetPos) {
					tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
					tempPos += paths[currentDir];
				}
			}
			//Doors are not aligned
			else {
				//Set current door's direction
				int dirMod = std::abs(doorDist.x) + std::abs(doorDist.y);
				int curDir = rng() % dirMod;
				if (curDir < std::abs(doorDist.x)) {
					//Door faces horizontally
					if (doorDist.x > 0) {
						currentDir = east;
					}
					else {
						currentDir = west;
					}
				}
				else {
					//Door faces vertically
					if (doorDist.y > 0) {
						currentDir = south;
					}
					else {
						currentDir = north;
					}
				}
				//Set target door's direction
				int tarDir = rng() % dirMod;
				if (tarDir < std::abs(doorDist.x)) {
					//Door faces horizontally
					if (doorDist.x > 0) {
						targetDir = west;
					}
					else {
						targetDir = east;
					}
				}
				else {
					//Door faces vertically
					if (doorDist.y > 0) {
						targetDir = north;
					}
					else {
						targetDir = south;
					}
				}
				//Check if hall uses 3 links, choose the connecting link
				if (currentDir + targetDir == 1) {
					//Horizontal doors, vertical link
					int linkMod = std::abs(doorDist.x);
					int linkDist = rng() % linkMod;
					//Link 1
					for (int x = 0; x < linkDist; ++x) {
						tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
						tempPos += paths[currentDir];
					}
					//Link 2
					dir link2Dir;
					if (doorDist.y > 0) {
						link2Dir = south;
					}
					else {
						link2Dir = north;
					}
					for (int y = 0; y < std::abs(doorDist.y); ++y) {
						tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
						tempPos += paths[link2Dir];
					}
					//Link 3
					for (int x = 0; x < linkMod - linkDist; ++x) {
						tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
						tempPos += paths[currentDir];
					}
				}
				else if (currentDir + targetDir == 5) {
					//Vertical doors, horizontal link
					int linkMod = std::abs(doorDist.y);
					int linkDist = rng() % linkMod;
					//Link 1
					for (int y = 0; y < linkDist; ++y) {
						tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
						tempPos += paths[currentDir];
					}
					//Link 2
					dir link2Dir;
					if (doorDist.x > 0) {
						link2Dir = east;
					}
					else {
						link2Dir = west;
					}
					for (int x = 0; x < std::abs(doorDist.x); ++x) {
						tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
						tempPos += paths[link2Dir];
					}
					//Link 3
					for (int y = 0; y < linkMod - linkDist; ++y) {
						tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
						tempPos += paths[currentDir];
					}
				}
				else {
					//Hall has 2 links (L shaped)
					int link1Dist;
					int link2Dist;
					if (currentDir == east || currentDir == west) {
						//Traveling horizontally first from current position
						link1Dist = std::abs(doorDist.x);
						link2Dist = std::abs(doorDist.y);
					}
					else {
						//Traveling vertically first from current position
						link1Dist = std::abs(doorDist.y);
						link2Dist = std::abs(doorDist.x);
					}
					//Link 1
					for (int i = 0; i <= link1Dist; ++i) {
						tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
						tempPos += paths[currentDir];
					}
					//Link 2
					tempPos = targetPos;
					for (int i = 0; i < link2Dist; ++i) {
						tempHall.insert(intXYtoN(tempPos.x, tempPos.y));
						tempPos += paths[targetDir];
					}
				}
			}
			//Make temp hall tiles into ground tiles
			for (int tile : tempHall) {
				tiles[tile] = ground;
			}
			//Add connected rooms to each other's lists
			for (room * r : targetRoom->connectedRooms) {
				currentRoom->connectedRooms.insert(r);
			}
			targetRoom->connectedRooms = currentRoom->connectedRooms;
			//Set connectedCount to this size, if it's bigger
			int connected = currentRoom->connectedRooms.size();
			if (connected > connectedCount) {
				connectedCount = connected;
			}
		}
	}
}

void Map::hallsWeightedProbs() {
	//Create halls in steps by weighting probabilities based on the other rooms
		//Approach:
		//Extend a door based on a Markov chain, probabilities based on distance to other rooms
		//Treat rooms as masses with a center of gravity, use x and y component vectors (with 0,0 as the current tile)
		//Ground tiles and doors of origin room should have negative gravity (the goal is to move away from this!)
		//Border tiles also have fixed negative gravity. Traveled tiles and their adjacent tiles are 0 probability
		//No need to normalize totals, since rng produces a big result. Use a modulus of sum of 4 path probabilities
		//Consider clamping negatives to 0, or add to all 4 paths such that the lowest is 0
		//Cornered paths are canceled, start a new search

	//Convenience parameters
	typedef std::pair<sf::Vector2i, room*> doorType;
	sf::Vector2i paths[4] = {
		sf::Vector2i(1,0),
		sf::Vector2i(-1,0),
		sf::Vector2i(0,1),
		sf::Vector2i(0,-1)
	};
	struct hall {
		std::set<int> hallTiles;
		room * originRoom;
		hall(std::set<int> tiles, room * roomPtr) : hallTiles(tiles), originRoom(roomPtr) {}
	};
	//Algorithm parameters
	std::vector<doorType> doors;
	std::list<hall> halls;
	const int borderBuffer = 1; //The space between room and outer border needs to be at least 1
	//Start algorithm
	if (!rooms.empty()) {
		int roomCount = rooms.size();
		int connectedCount = 1;
		//Determine door locations (determined by the bordering wall tiles)
		for (room * r : rooms) {
			//Find top and bottom doors
			for (int y = r->position.y - 1; y <= r->position.y + r->size.y; y += r->size.y + 1) {
				for (int x = r->position.x; x < r->position.x + r->size.x; ++x) {
					//Add door if location is valid
					if (x > borderBuffer && y > borderBuffer && x < tilesPerRow - borderBuffer - 1 && y < tilesPerCol - borderBuffer - 1) {
						doorType newDoor = { sf::Vector2i(x,y), r };
						doors.push_back(newDoor);
						r->doorList.push_back(newDoor.first);
						//tiles[intXYtoN(x, y)] = water;
					}
				}
			}
			//Find left and right doors
			for (int x = r->position.x - 1; x <= r->position.x + r->size.x; x += r->size.x + 1) {
				for (int y = r->position.y; y < r->position.y + r->size.y; ++y) {
					//Add door if location is valid
					if (x > borderBuffer && y > borderBuffer && x < tilesPerRow - borderBuffer - 1 && y < tilesPerCol - borderBuffer - 1) {
						doorType newDoor = { sf::Vector2i(x,y), r };
						doors.push_back(newDoor);
						r->doorList.push_back(newDoor.first);
						//tiles[intXYtoN(x, y)] = water;
					}
				}
			}
		}
		int doorCount;
		doorType currentDoor;
		std::set<int> tempHall;
		float pathProbs[4] = { 0,0,0,0 }; //Direction: 0 +x, 1 -x, 2 +y, 3 -y
		bool buildingHall;
		while (connectedCount < roomCount) {
			//Choose a random door
			doorCount = doors.size();
			if (doorCount == 0)
				break;
			currentDoor = doors[rng() % doorCount];
			room * currentRoom = currentDoor.second;
			//Begin building hallway
			buildingHall = true;
			tempHall.clear();
			sf::Vector2i tempDoor = currentDoor.first; //This is the test location of the hall, as if the door extruded outwards
			tempHall.insert(intXYtoN(tempDoor.x, tempDoor.y));
			sf::Vector2f force;
			int lastDir = 4; //Index for the last direction traveled. Starts at 4 so that first pass doesn't use this
			while (buildingHall) {
				//Reset probabilities
				for (int i = 0; i < 4; ++i)
					pathProbs[i] = 0;
				//Find gravity along x and y
				for (room * r : rooms) {
					float mass = r->size.x * r->size.y;
					if (currentDoor.second == r) {
						//Origin room, so negate mass to move away rapidly
						mass *= -1;
					}
					sf::Vector2f roomCenter = { r->position.x + r->size.x / 2.f, r->position.y + r->size.y / 2.f };
					sf::Vector2f doorCenter = { tempDoor.x + 0.5f, tempDoor.y + 0.5f };
					sf::Vector2f distance = roomCenter - doorCenter;
					float distanceMag = std::sqrtf(std::powf(distance.x, 2.f) + std::powf(distance.y, 2.f));
					float forceMag = mass / std::powf(distanceMag, 2.f);
					sf::Vector2f forceDir = distance / distanceMag;
					force = forceMag * forceDir;
					//Find force to increase probability of traveling in a direction. Sign of distance chooses which path is increased
					if (distance.x > 0) {
						pathProbs[0] += std::copysignf(force.x, mass);
					}
					else if (distance.x < 0) {
						pathProbs[1] += std::copysignf(force.x, mass);
					}
					if (distance.y > 0) {
						pathProbs[2] += std::copysignf(force.y, mass);
					}
					else if (distance.y < 0) {
						pathProbs[3] += std::copysignf(force.y, mass);
					}
				}
				//Set all probabilities to non-negative by subtracting most negative number
				float offset = *std::min_element(pathProbs, pathProbs + 4);
				if (offset < 0) {
					for (int i = 0; i < 4; ++i) {
						pathProbs[i] -= offset;
					}
				}
				//Impose constraints and add to total for rng modulus
				int dirMod = 0;
				sf::Vector2i tempPath;
				for (int i = 0; i < 4; ++i) {
					tempPath = tempDoor + paths[i];
					//Is the path a floor tile of the origin room?
					if (tempPath > currentRoom->position && tempPath < currentRoom->position + currentRoom->size) {
						pathProbs[i] = 0;
						continue;
					}
					//Is the path a door of the origin room?
					if (std::find(currentRoom->doorList.begin(), currentRoom->doorList.end(), tempPath) != currentRoom->doorList.end()) {
						pathProbs[i] = 0;
						continue;
					}
					//Is the path a border tile?
					if (tempPath.x == 0 || tempPath.x == tilesPerRow - 1 || tempPath.y == 0 || tempPath.y == tilesPerCol - 1) {
						pathProbs[i] = 0;
						continue;
					}
					//Is the path on an explored tile?
					if (tempHall.find(intXYtoN(tempPath.x, tempPath.y)) != tempHall.end()) {
						pathProbs[i] = 0;
						continue;
					}
					//Add a bonus if this is the same direction as last step
					if (lastDir == i)
						pathProbs[i] *= 2.f;
					//Increase total modulus for the roll. If a number to add is 0, this getting skipped is inconsequential
					dirMod += int(1000 * pathProbs[i]);
				}
				//Roll to check the direction
				if (dirMod > 0) {
					int dir = rng() % dirMod;
					int dirCheck = 0;
					for (int i = 0; i < 4; ++i) {
						//Increase the check
						dirCheck += int(1000 * pathProbs[i]);
						//The rng rolled more than last check, but lower than this check, so end loop and use new path
						if (dir < dirCheck) {
							tempDoor += paths[i];
							tempHall.insert(intXYtoN(tempDoor.x, tempDoor.y));
							//Update last direction with this new one
							lastDir = i;
							break;
						}
					}
					//Check if this new location is a hall
					for (std::list<hall>::iterator it = halls.begin(); it != halls.end(); ++it) {
						hall h = *it;
						if (h.hallTiles.find(intXYtoN(tempDoor.x, tempDoor.y)) != h.hallTiles.end()) {
							//Hall connected to hall, end this while loop
							buildingHall = false;
							//Add the connecting rooms' set of rooms to each other
							for (room * r : h.originRoom->connectedRooms) {
								currentRoom->connectedRooms.insert(r);
							}
							h.originRoom->connectedRooms = currentRoom->connectedRooms;
							//Set connectedCount to this size, if it's bigger
							int connected = currentRoom->connectedRooms.size();
							if (connected > connectedCount) {
								connectedCount = connected;
							}
							//Set all tiles in the temporary hall to actual ground tiles
							for (int t : tempHall) {
								tiles[t] = ground;
							}
							//Add the hall to the list
							halls.emplace_back(tempHall, currentRoom);
							//Remove the door from the vector
							doors.erase(std::find(doors.begin(), doors.end(), currentDoor));
							break;
						}
					}
					//No reason to check if the location is a door now that the hall is built
					if (!buildingHall)
						continue;
					//Check if this new location is a door of another room
					for (std::vector<doorType>::iterator it = doors.begin(); it != doors.end(); ++it) {
						doorType d = *it;
						if (d.first == tempDoor && d.second != currentRoom) {
							//Hall complete, end this while loop
							buildingHall = false;
							//Add the connecting rooms' set of rooms to each other
							for (room * r : d.second->connectedRooms) {
								currentRoom->connectedRooms.insert(r);
							}
							d.second->connectedRooms = currentRoom->connectedRooms;
							//Set connectedCount to this size, if it's bigger
							int connected = currentRoom->connectedRooms.size();
							if (connected > connectedCount) {
								connectedCount = connected;
							}
							//Set all tiles in the temporary hall to actual ground tiles
							for (int t : tempHall) {
								tiles[t] = ground;
							}
							//Add the hall to the list
							halls.emplace_back(tempHall, currentRoom);
							//Remove the doors from the vector
							doors.erase(it);
							doors.erase(std::find(doors.begin(), doors.end(), currentDoor));
							break;
						}
					}
				}
				//If dirMod is 0, then the hallway hit a dead end
				else {
					buildingHall = false;
				}
			}
		}
	}
}
