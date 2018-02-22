//
// A* path finding algorithm
// created by Rukiya 1/26/2018
// heavily based off of https://github.com/daancode/a-star/blob/master/source/AStar.cpp

#include "AStar.h"
#include <algorithm>

using namespace std::placeholders;

AStar::Node::Node(glm::vec2 aCoordinates, Node *aParent) {
	parent = aParent;
	coordinates = aCoordinates;
	G = H = 0;
}


AStar::uint AStar::Node::getScore() {
	return G + H;
}


// heuristic is euclidean, manhattan, or octagonal
// square grid + moving diagonally
AStar::Generator::Generator() {
	//setDiagonalMovement(true);
	setHeuristic(&Heuristic::manhattan);
	direction = {
		{0,1}, {1,0}, {0,-1},{-1,0},
		{-1,-1}, {1,1}, {-1,1}, {1,-1}
	};
}

// based on generated map
void AStar::Generator::setWorldSize(glm::vec2 aWorldSize) {
	worldSize = aWorldSize;
}

// square grid, shouldn't need this?
//void AStar::Generator::setDiagonalMovement(bool enable) {
//	directions = (enable ? 8 : 4);
//}


void AStar::Generator::setHeuristic(HeuristicFunction aHeuristic) {
	heuristic = std::bind(aHeuristic, _1, _2); 
}

// unbreakable objects, add another for breakable later?
// based on generated map
void AStar::Generator::addCollision(glm::vec2 coordinates) {
	walls.push_back(coordinates);
}

void AStar::Generator::addCrystal(glm::vec2 coordinates) {
	crystals.push_back(coordinates);
}

void AStar::Generator::removeCrystal(glm::vec2 coordinates) {
	auto it = std::find(crystals.begin(), crystals.end(), coordinates);
	if (it != crystals.end()) {
		crystals.erase(it);
	}
}

void AStar::Generator::removeCollision(glm::vec2 coordinates) {
	auto it = std::find(walls.begin(), walls.end(), coordinates);
	if (it != walls.end()) {
		walls.erase(it);
	}
}

void AStar::Generator::clearCollisions() {
	walls.clear();
}


// the actual path finding algorithm
AStar::CoordinateList AStar::Generator::findPath(glm::vec2 source, glm::vec2 target) {
	Node *current = nullptr;
	NodeSet openSet, closedSet;
	openSet.insert(new Node(source));

	while (!openSet.empty()) {
		current = *openSet.begin();
		// for all nodes in openSet, get one with smallest score
		// which is shortest path
		for (auto node : openSet) {
			// if length is the shortest
			if (node->getScore() <= current->getScore()) {
				current = node;	// this will be node to go to
			}
		}
		
		// check first if arrived at destination
		if (current->coordinates == target) {
			break;
		}

		closedSet.insert(current);
		openSet.erase(std::find(openSet.begin(), openSet.end(), current));

		// search each direction
		for (uint i = 0; i < directions; ++i) {
			// get coordinates of each possible direction
			glm::vec2 newCoordinates(current->coordinates + direction[i]);
			// don't consider coordinate if is collision or already on list
			if (detectCollision(newCoordinates) || findNodeOnList(closedSet, newCoordinates) ||
				detectCrystal(newCoordinates)) {
				continue;
			}

			// get total cost
			// if i==4 then diagonal begins
			//uint totalCost = current->G + ((i < 4) ? 10 : 14);
			uint totalCost = current->G + 10;

			Node *successor = findNodeOnList(openSet, newCoordinates);
			if (successor == nullptr) {
				successor = new Node(newCoordinates, current);
				successor->G = totalCost;
				successor->H = heuristic(successor->coordinates, target);
				openSet.insert(successor);
			}
			// backtrack if all other options too big of cost
			else if (totalCost < successor->G) {
				successor->parent = current;
				successor->G = totalCost;
			}
		}
	}

	// get the ideal path into a list
	CoordinateList path;
	while (current != nullptr) {
		path.push_back(current->coordinates);
		current = current->parent;
	}

	// clean up
	releaseNodes(openSet);
	releaseNodes(closedSet);

	return path;
}

AStar::Node* AStar::Generator::findNodeOnList(NodeSet& aNodes, glm::vec2 aCoordinates) {
	for (auto node : aNodes) {
		if (node->coordinates == aCoordinates) {
			return node;
		}
	}
	return nullptr;
}

void AStar::Generator::releaseNodes(NodeSet& nodes) {
	for (auto it = nodes.begin(); it != nodes.end();) {
		delete *it;
		it = nodes.erase(it);
	}
}

bool AStar::Generator::detectCollision(glm::vec2 coordinates) {
	if (coordinates.x <= -worldSize.x || coordinates.x >= worldSize.x ||
		coordinates.y <= -worldSize.y || coordinates.y >= worldSize.y ||
		std::find(walls.begin(), walls.end(), coordinates) != walls.end()) {
		return true;
	}
	return false;
}

bool AStar::Generator::detectCrystal(glm::vec2 coordinates) {
	if (coordinates.x <= -worldSize.x || coordinates.x >= worldSize.x ||
		coordinates.y <= -worldSize.y || coordinates.y >= worldSize.y ||
		std::find(crystals.begin(),crystals.end(), coordinates) != crystals.end()) {
		return true;
	}
	return false;
}

glm::vec2 AStar::Heuristic::getDelta(glm::vec2 source, glm::vec2 target) {
	return glm::vec2(abs(source.x - target.x), abs(source.y - target.y));
}


AStar::uint AStar::Heuristic::euclidean(glm::vec2 source, glm::vec2 target) {
	auto delta = std::move(getDelta(source, target));
	return static_cast<uint>(10 * sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
}

AStar::uint AStar::Heuristic::manhattan(glm::vec2 source, glm::vec2 target)
{
	auto delta = std::move(getDelta(source, target));
	return static_cast<uint>(10 * (delta.x + delta.y));
}

