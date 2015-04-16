#pragma once

#include <glm/glm.hpp>

class collisionDetector
{
public:
	collisionDetector(void);
	~collisionDetector(void);

	/*struct Simplex{
		vec3 pointA;
		vec3 pointB;
		vec3 minkowskiDifference;
	};*/

	bool planeCollision(glm::vec3& p1, glm::vec3& p2, float threshold, glm::vec3& intersect, glm::vec3& normal);

	bool sphereCollision(glm::vec3& p1, glm::vec3& p2, float threshold, glm::vec3& intersect, glm::vec3& normal, float radius, glm::vec3 center);
};

