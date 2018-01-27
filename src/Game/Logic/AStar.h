//
// created by Rukiya 1/26/2018
// heavily based off of https://github.com/daancode/a-star/blob/master/source/AStar.hpp
//
#pragma once
#ifndef __ASTAR_H
#define __ASTAR_H

#include <glm/vec2.hpp>
#include <vector>
#include <functional>
#include <set>

using namespace glm;

namespace AStar {
	
	using uint = unsigned int;
	using HeuristicFunction = std::function<uint(vec2, vec2)>;
	using CoordinateList = std::vector<vec2>;

	struct Node {
		uint G, H;
		vec2 coordinates;
		Node *parent;
		Node(vec2 aCoordinates, Node *aParent = nullptr);
		uint getScore();
	};

	using NodeSet = std::set<Node*>;

	class Generator {
		bool detectCollision(vec2 coordinates);
		Node* findNodeOnList(NodeSet& aNodes, vec2 aCoordinates);
		void releaseNodes(NodeSet& nodes);

	public:
		Generator();
		void setWorldSize(vec2 aWorldSize);
		void setDiagonalMovement(bool enable);
		void setHeuristic(HeuristicFunction aHeuristic);
		CoordinateList findPath(vec2 source, vec2 target);
		void addCollision(vec2 coordinates);
		void removeCollision(vec2 coodrinates);
		void clearCollisions();

	private:
		HeuristicFunction heuristic;
		CoordinateList direction, walls; // add destructableWalls later
		vec2 worldSize;
		uint directions;
	};

	class Heuristic {
		static vec2 getDelta(vec2 source, vec2 target);

	public:
		static uint euclidean(vec2 source, vec2 target);
	};
	
}
#endif // __ASTAR_H