

//
// created by Rukiya 2/03/2018

// heuristic is euclidean, manhattan, or octagonal (um last two not added)
// square grid + moving diagonally
AStar::Generator::Generator() {
	//setDiagonalMovement(true);
	setHeuristic(&Heuristic::euclidean);
	direction = {
		{ 0,1 },{ 1,0 },{ 0,-1 },{ -1,0 },
		{ -1,-1 },{ 1,1 },{ -1,1 },{ 1,-1 }
	};
}

void AStar::Generator::setWorldSize(vec2 aWorldSize) {
	worldSize = aWorldSize;
}

// square grid, shouldn't need this?
//void AStar::Generator::setDiagonalMovement(bool enable) {
//	directions = (enable ? 8 : 4);
//}


void AStar::Generator::setHeuristic(HeuristicFunction aHeuristic) {
	heuristic = std::bind(aHeuristic, _1, _2);
}

// unbreakable objects, add another for breakable later
// based on generated map
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




int nextPoint = path.size() - 1;
vec2 target = path[nextPoint];
vec2 toTargetVector;
vec2 newVector;
vec2 prevPos = vec2(x, y);
vec2 velocity;




decompose(entity1->getMatrix(), scale, rot, pos, skew, perspective);
vehicleFacingVector = conjugate(rot) * forwardVector; // direction vehicle is facing

x = (pos.x - fmod(pos.x, gridSize)) / gridSize;
y = (pos.z - fmod(pos.z, gridSize)) / gridSize;
toTargetVector = normalize(target - vec2(x, y));	// direction pointing to target

std::cout << "current point::: " << x << ", " << y << std::endl;
std::cout << "target point::: " << target.x << ", " << target.y << std::endl;
std::cout << "direction to go::: " << toTargetVector.x << ", " << toTargetVector.y << std::endl;
std::cout << "forward::: " << vehicleFacingVector.x << ", " << vehicleFacingVector.z << std::endl;


// if met target, get new target

if (x == target.x && y == target.y) {
	nextPoint--;
	if (nextPoint < 0) {
		// if nextPoint is -1, reached end, so stop
		tempPlayerInput.brake = true;
	}
	else {
		//std::cout << "Next grid target " << nextPoint << std::endl;
		target = path[nextPoint];
	}
}

// try to turn car	
//speed = velocity(prevPos, vec2(pos.x, pos.z));
//std::cout << "speed " << abs(speed) << std::endl;
//std::cout << " current " << pos.x << " " << pos.z << std::endl;
//std::cout << "previous " << prevPos.x << " " << prevPos.y << std::endl;

// turn car by having its own direction vector equal the target vector (which points directly to target)
//velocity = vec2(abs(pos.x - prevPos.x) * 100, abs(pos.z - prevPos.y) * 100);
//std::cout << "speed " << velocity.x << ", " << velocity.y << std::endl;

if ((x != target.x) || (y != target.y)) {
	// if incorrect X position in graph
	if (x != target.x) {
		if (vehicleFacingVector.x < toTargetVector.x) {
			tempPlayerInput.steerLeft = true;
		}
		else if (vehicleFacingVector.x > toTargetVector.x) {
			tempPlayerInput.steerRight = true;
		}
	}

	// if incorrect Y position in graph
	if (y != target.y) {
		if (vehicleFacingVector.z > toTargetVector.y) {
			tempPlayerInput.steerLeft = true;
		}
		else if (vehicleFacingVector.z < toTargetVector.y) {
			tempPlayerInput.steerRight = true;
		}
	}

	tempPlayerInput.accel = true;
}

prevPos = vec2(pos.x, pos.z);



tempPlayerInput.steerRight = tempPlayerInput.steerLeft = tempPlayerInput.accel = tempPlayerInput.brake = tempPlayerInput.handbrake = false;
