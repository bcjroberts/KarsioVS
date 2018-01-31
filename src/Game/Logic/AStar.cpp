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



struct Triangle {
	int index;
	vec3 a, b, c;
	std::vector<int> adjTris;	// list of adjacent triangles by index
};



struct Point {
	int index;
	vec3 pos;
	std::vector<int> adjNodes;
};

std::vector<Triangle> triangles;
std::vector<Point> points;

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

bool pointInTriangle(vec3 p, vec3 a, vec3 b, vec3 c) {
	if (sameSide(p, a, b, c) && sameSide(p, b, a, c) && sameSide(p, c, a, b)) {
		return true;
	}
	else {
		return false;
	}

}

void findPath(std::vector<vec3> vertices, vec3 start, vec3 end) {
	std::vector<vec3> openList;
	std::vector<vec3> closedList;
	vec3 current = start;
	bool startInTri = false;
	bool endInTri = false;
	int startIndex = -1;
	int endIndex = -1;
	
	// check where you are is in a triangle
	// for all triangles, check which triangle point is in
	// if either start or end are not in a triangle, stop the entire operation
	for (int i = 0; i < triangles.size(); i++) {
		// each 3 in vertices form a triangle
		startInTri = pointInTriangle(start, triangles[i].a, triangles[i].b, triangles[i].c);
		if (startInTri) {
			// confirm that startindex is index of triangles
			startIndex = i;
			break;
		}
		if (!startInTri) {
			return;
		}
	}
	// check if place you want to go is in a triangle
	for (int i = 0; i < triangles.size(); i++) {
		// each 3 in vertices form a triangle
		endInTri = pointInTriangle(end, triangles[i].a, triangles[i].b, triangles[i].c);
		//endInTri = pointInTriangle(start, vertices[i], vertices[i + 1], vertices[i + 2]);
		if (endInTri) {
			// confirm that startindex is index of triangles
			endIndex = i;
			break;
		}
		if (!endInTri) {
			return;
		}
	}

	// reached here meaning both points are in a triangle
	// check if both in same triangle
	if (startIndex == endIndex) {
		// in same triangle, go straight to it
	}
	// if reaches else, both points located on different triangles
	else {
		// keep record of parent point (which is start)
		closedList.push_back(start);

		// calculation start

		int currentIndex = startIndex;
		float cost = 0;
		std::vector<float> G;
		int j = 0;

		while (currentIndex != endIndex) {

			// send all adjacent points to open list
			for (int i = 0; i < triangles[currentIndex].adjTris.size() - 1; i++) {
				// get index of adjacent triangle
				j = triangles[currentIndex].adjTris[i];
				// add its vertices
				if (current != triangles[j].a) {
					openList.push_back(triangles[j].a);
				}
				if (current != triangles[j].b) {
					openList.push_back(triangles[j].b);
				}
				if (current != triangles[j].c) {
					openList.push_back(triangles[j].c);
				}
			}


			// calculate F, where F = G + H
			// G = cost from start to current node
			// H = cost of getting to end from current node
			// look at each point, then compare
			for (int i = 0; i > openList.size()-1; i++) {
				//G[i] = calculateCost(start, openList.at[i], end);
			}

			float F = 0;
			// get minimum F cost
			for (int i = 0; i < G.size() - 1; i++) {
				if ((i + 1) == G.size()) {
					F = min(G[0], G[i]);
				}
				else {
					F = min(G[i], G[i + 1]);
				}
			}
			
			// put the shortest path into closed list
			for (int i = 0; i < G.size() - 1; i++) {
				if (F == G[i]) {
					//currentIndex = i;
					//closedList.push_back(openList.at[currentIndex]);
					//current = openList.at[currentIndex];
				}
			}
			G.clear();
			cost += F;
		}


	}	// end of path finding (in the else)
}

// a = start
// b = possible node to move to
// c = end
float calculateCost(vec3 a, vec3 b, vec3 c) {
	float G = abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
	float H = abs(b.x - c.x) + abs(b.y - c.y) + abs(b.z - c.z);

	return G + H;
}

void AStar::Generator::importNavmesh() {
	MeshData navMesh("navmesh");
	navMesh.loadMeshData("data/assets/meshes/cube.obj");
	
	Triangle tri;
	int j = 0;

	// every 3 points makes one triangle
	// after every three iterations, put in a triangle struct node thing
	for (int i = 0; i < navMesh.vertices.size(); i += 3) {
		tri.index = j++;
		tri.a = navMesh.vertices[i].position;
		tri.b = navMesh.vertices[i+1].position;
		tri.c = navMesh.vertices[i+2].position;
		triangles.push_back(tri);		
		}
	int adj = 0;
	// find all adjacent triangles
	for (int i = 0; i < triangles.size() - 1; i++) {
		for (int m = 0; m < triangles.size() - 1; m++) {
			adj = 0;
			if (triangles[i].index == triangles[m].index) {
				continue;
			}
			// check if neighbours by if triangles share two vertices
			if (triangles[i].a == triangles[m].a || triangles[i].a == triangles[m].b || triangles[i].a == triangles[m].c) {
				adj++;
			}

			if (triangles[i].b == triangles[m].a || triangles[i].b == triangles[m].b || triangles[i].b == triangles[m].c) {
				adj++;
			}

			if (triangles[i].c == triangles[m].a || triangles[i].c == triangles[m].b || triangles[i].c == triangles[m].c) {
				adj++;
			}
			if (adj == 2) {
				// then it's a neighbour triangle woo...
				triangles[i].adjTris.push_back(m);
			}
		}
	}
	
		//std::cout << "i = " << i << " ";
		//std::cout << tempPos.x << ":"<< tempPos.y <<":"<< tempPos.z << std::endl
	
}





AStar::Node::Node(vec2 aCoordinates, Node *aParent) {
	parent = aParent;
	coordinates = aCoordinates;
	G = H = 0;
}


AStar::uint AStar::Node::getScore() {
	return G + H;
}

 
// heuristic is euclidean, manhattan, or octoganol
// can change direction from square grid to other-sided grid here
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

// square grid; can change for hexagonal etc
void AStar::Generator::setDiagonalMovement(bool enable) {
	directions = (enable ? 8 : 4);
}


void AStar::Generator::setHeuristic(HeuristicFunction aHeuristic) {
	heuristic = std::bind(aHeuristic, _1, _2); 
}

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

