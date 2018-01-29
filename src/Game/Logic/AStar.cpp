//
// A* path finding algorithm
// created by Rukiya 1/26/2018
// heavily based off of https://github.com/daancode/a-star/blob/master/source/AStar.cpp

#include "AStar.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <map>

#include "../../Art/MeshData.h"

using namespace std::placeholders;


struct Graph {
	std::map<char, std::vector<char>> edges;
	std::vector<char> neighbours(char id) {
		return edges[id];
	}
};

struct Location {

};

void importNavmesh() {
	MeshData navMesh("navmesh");
	navMesh.loadMeshData("data/assets/meshes/naviTest.obj");
	
	Graph graph;

	// get x y z coordinates... we already have them
	// if each x y z is the vertices then for loop isn't needed
	vec3 tempPos;
	std::vector<vec3> vertexPoints;

	// get list of edges



	for (unsigned int i = 0; i < tempPos.length; i++) {
		tempPos = navMesh.vertices[i].position;
		vertexPoints.push_back(vec3(tempPos.x, tempPos.y, tempPos.z));
	}
	
}

bool sameSide(vec3 p1, vec3 p2, vec3 a, vec3 b) {
	vec3 cp1 = cross(b - a, p1 - a);
	vec3 cp2 = cross(b - a, p2 - a);
	if (dot(cp1, cp2) >= 0) {
		return true;
	}
	else {
		return false;
	}
}

bool pointInTriange(vec3 p, vec3 a, vec3 b, vec3 c) {
	if (sameSide(p, a, b, c) && sameSide(p, b, a, c) && sameSide(p, c, a, b)) {
		return true;
	}
	else {
		return false;
	}

}

inline double heuristic(vec2 a, vec2 b) {
	return abs(a.x - b.x) + abs(a.y - b.y);
}


template<typename Location, typename Graph>
void a_star_search(Graph graph, Location start, Location goal,
	std::map<Location, Location>& came_from,
	std::map<Location, double>& cost_so_far) {
	PriorityQueue<Location, double> frontier;
	frontier.put(start, 0);

	came_from[start] = start;
	cost_so_far[start] = 0;

	while (!frontier.empty()) {
		Location current = frontier.get();

		if (current == goal) {
			break;
		}

		for (Location next : graph.neighbours(current)) {
			double new_cost = cost_so_far[current] + graph.cost(current, next);
			if (cost_so_far.find(next) == cost_so_far.end() ||
				new_cost < cost_so_far[next]) {
				cost_so_far[next] = new_cost;
				double priority = new_cost + heuristic(next, goal);
				frontier.put(next, priority);
				came_from[next] = current;
			}
		}
	}
}

// build path to be used
template<typename Location>
std::vector<Location> reconstruct_path(Location start, Location goal, std::map<Location, Location> came_from) {
	std::vector<Location> path;
	Location current = goal;
	while (current != start) {
		path.push_back(current);
		current = came)from[current];
	}
	path.push_back(start); // optional
	std::reverse(path.begin(), path.end());
	return path;
}

AStar::Node::Node(vec2 aCoordinates, Node *aParent, int aCost) {
	parent = aParent;
	coordinates = aCoordinates;
	cost = aCost;
	G = H = 0;
}


AStar::uint AStar::Node::getScore() {
	return G + H;
}

// constructor... 
// heuristic is euclidean, manhattan, or octoganol
AStar::Generator::Generator() {
	setDiagonalMovement(false);
	setHeuristic(&Heuristic::euclidean);
	direction = {
		{0,1}, {1,0}, {0,-1},{-1,0},
		{-1,-1}, {1,1}, {-1,1}, {1,-1}
	};
}

void AStar::Generator::setWorldSize(vec2 aWorldSize) {
	worldSize = aWorldSize;
}

// shouldn't need this
void AStar::Generator::setDiagonalMovement(bool enable) {
	directions = (enable ? 8 : 4);
}


void AStar::Generator::setHeuristic(HeuristicFunction aHeuristic) {
	heuristic = std::bind(aHeuristic, _1, _2); 
}

// shouldn't need this
// unbreakable objects, add another for breakable later
void AStar::Generator::addCollision(vec2 coordinates) {
	walls.push_back(coordinates);
}

void AStar::Generator::removeCollision(vec2 coordinates) {
	auto it = std::find(walls.begin(), walls.end(), coordinates);
	if (it != walls.end()) {
		walls.erase(it);
	}
}

void AStar::Generator::clearCollisions() {
	walls.clear();
}


// the actual path algorithm
AStar::CoordinateList AStar::Generator::findPath(vec2 source, vec2 target) {
	Node *current = nullptr;
	NodeSet openSet, closedSet;
	openSet.insert(new Node(source));

	while (!openSet.empty()) {
		current = *openSet.begin();
		// for all nodes in openSet, get one with smallest score
		// which is shortest path
		// probably change to length instead, distance from one vertex to another
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

		// search each directiion
		// instead, search each adjacent vertex
			// get distance
			// continue rest of algorithm?
		for (uint i = 0; i < directions; ++i) {
			// get coordinates of each possible direction
			vec2 newCoordinates(current->coordinates + direction[i]);
			// don't consider coordinate if is collision or already on list
			if (detectCollision(newCoordinates) || findNodeOnList(closedSet, newCoordinates)) {
				continue;
			}

			// get total cost
			// i think it makes diagonal extra cost,
			// this is where cost is made
			// if i==4 then diagonal begins

			uint totalCost = current->G + ((i < 4) ? 10 : 14);
			// do the triangle thing
			//uint totalCost = get cost somehow;	// may not need this after all, cost calculated at start
			// new node created
			// need to get cost this way from somewhere else
			Node *successor = findNodeOnList(openSet, newCoordinates);
			if (successor == nullptr) {
				successor = new Node(newCoordinates, current);
				successor->G = totalCost;
				successor->H = heuristic(successor->coordinates, target);
				successor->cost = totalCost;	// may not need this after all, cost calculated at start
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

AStar::Node* AStar::Generator::findNodeOnList(NodeSet& aNodes, vec2 aCoordinates) {
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

bool AStar::Generator::detectCollision(vec2 coordinates) {
	if (coordinates.x < 0 || coordinates.x >= worldSize.x ||
		coordinates.y < 0 || coordinates.y >= worldSize.y ||
		std::find(walls.begin(), walls.end(), coordinates) != walls.end()) {
		return true;
	}
	return false;
}

vec2 AStar::Heuristic::getDelta(vec2 source, vec2 target) {
	return vec2(abs(source.x - target.x), abs(source.y - target.y));
}


AStar::uint AStar::Heuristic::euclidean(vec2 source, vec2 target) {
	auto delta = std::move(getDelta(source, target));
	return static_cast<uint>(10 * sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
}

