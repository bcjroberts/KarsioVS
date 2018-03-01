//
// created by Rukiya 1/26/2018
// heavily based off of https://github.com/daancode/a-star/blob/master/source/AStar.hpp
//
#pragma once
#ifndef __ASTAR_H
#define __ASTAR_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <functional>
#include <set>

namespace AStar {
	
	using uint = unsigned int;
	using HeuristicFunction = std::function<uint(glm::vec2, glm::vec2)>;
	using CoordinateList = std::vector<glm::vec2>;

	struct Node {
		uint G, H;
		glm::vec2 coordinates;
		Node *parent;
		Node(glm::vec2 aCoordinates, Node *aParent = nullptr);
		uint getScore();
	};

	using NodeSet = std::set<Node*>;

	class Generator {
		bool detectCollision(glm::vec2 coordinates);
		bool detectCrystal (glm::vec2 coordinates);
		Node* findNodeOnList(NodeSet& aNodes, glm::vec2 aCoordinates);
		void releaseNodes(NodeSet& nodes);

	public:
		Generator();
		void setWorldSize(glm::vec2 aWorldSize);
		//void setDiagonalMovement(bool enable);
		void setHeuristic(HeuristicFunction aHeuristic);
		CoordinateList findPath(glm::vec2 source, glm::vec2 target);
		void addCollision(glm::vec2 coordinates);
		void addCollision(glm::vec2 coordinates, glm::vec3 scale);
		void addCrystal(glm::vec2 coordinates);
		void removeCrystal(glm::vec2 coordinates);
		void removeCollision(glm::vec2 coodrinates);
		void clearCollisions();

	private:
		HeuristicFunction heuristic;
		CoordinateList direction, walls, crystals; // add destructable walls later?
		glm::vec2 worldSize;
		uint directions = 8;
	};

	class Heuristic {
		static glm::vec2 getDelta(glm::vec2 source, glm::vec2 target);

	public:
		static uint euclidean(glm::vec2 source, glm::vec2 target);
		static uint manhattan(glm::vec2 source, glm::vec2 target);
	};
	
}
#endif // __ASTAR_H