#include "collisionDetector.h"


collisionDetector::collisionDetector(void)
{
}


collisionDetector::~collisionDetector(void)
{
}

bool collisionDetector::planeCollision( glm::vec3& p1, glm::vec3& p2, float threshold, glm::vec3& intersect, glm::vec3& normal) 
{
	float v1, v2;
	glm::vec3 planeNormal = glm::vec3(0,1,0);
	v1 = glm::dot(p1, planeNormal);
	v2 = glm::dot(p2, planeNormal);
	if(v2 < threshold)//new position collision
	{
		normal = planeNormal;
		if(v1 >= threshold)//test if collision happended in last position
		{
			intersect = ((v1 - threshold) * p2 - (v2 - threshold) * p1) / (v1 - v2);
		}
		else
		{
			intersect = p2 - (v2 - threshold) * normal;
		}
		return true;
	}
	else
		return false;
}

bool collisionDetector::sphereCollision(glm::vec3& p1, glm::vec3& p2, float threshold, glm::vec3& intersect, glm::vec3& normal, float radius, glm::vec3 center)
{// TODO: implement line-sphere intersection. you can refer to line-plane intersection.
	float r = radius + 0.1f; // nudge out
	glm::vec3 p1c = p1 - center;
	glm::vec3 p2c = p2 - center;

	float v1, v2;// v1 v2 are distance to sphere for p1 and p2.

	v1 = glm::length(p1c) - r;
	v2 = glm::length(p2c) - r;
	if(v2 < threshold)
	{
		glm::vec3 diff = glm::normalize(p2 - p1);
		float A = glm::dot(diff, diff);
		float B = 2.0f * glm::dot(diff, p1c);
		float C = glm::dot(p1c, p1c) - pow(r, 2);

		float discriminant = pow(B, 2) - 4.0f * A * C;
		float t;

		if(v1 >= threshold)
		{
			if (discriminant < 0) {//没有非零解，方程式恒>0
				return false;
			}
			else if (discriminant == 0) {//存在唯一非零解,方程式=0
				t = -B/(2*A);
			}
			else {//存在不相等的两个非零解t1,t2
				float t1 = (-B - sqrt(discriminant)) / (2*A);
				float t2 = (-B + sqrt(discriminant)) / (2*A);

				if (t1 < t2 && t1 > 0) {
					t = t1;
				}
				else if (t2 > 0) {
					t = t2;
				}
				else {
					return false;
				}
			}

			intersect = p1 + t * diff;
			normal = glm::normalize(intersect - center);
		}
		else
		{
			if (discriminant < 0) {
				return false;
			}
			else if (discriminant == 0) {
				t = -B/(2*A);
			}
			else {
				float t1 = (-B - sqrt(discriminant)) / (2*A);
				float t2 = (-B + sqrt(discriminant)) / (2*A);

				if (abs(t1) < abs(t2)) {
					t = t1;
				}
				else {
					t = t2;
				}
			}

			intersect = p1 + t * diff;
			normal = glm::normalize(intersect - center);
		}
		return true;
	}
	else
		return false;
}

//bool CheckCollisionNarrow(Face &body1, Face &body2)
//{
//	//demoResult.clear();
//	std::vector<Simplex> simplex;
//
//	glm::vec3 direction = body1.p1->pos - body2.p1->pos;
//
//	simplex.push_back(support(direction, body1, body2));
//
//	direction = -simplex[0].minkowskiDifference;
//	int counter = 100;
//
//	while (counter > 0)
//	{
//		Simplex tempSimplex;
//		tempSimplex = support(direction, body1, body2);
//
//		// Last point added was not past the origin in this direction
//		if(glm::dot(tempSimplex.minkowskiDifference, direction) < 0)
//		{
//			return false;
//		}
//		simplex.push_back(tempSimplex);
//
//		//check intersect
//		if (processSimplex(simplex, direction))
//		{
//			return true;
//		}
//		counter--;
//	}
//}
//
//Simplex support(glm::vec3 direction, Triangle &body1, Triangle &body2)
//{
//	Simplex s;
//	s.pointA = getFarthestPointInDirection(direction, body1.v);
//	s.pointB = getFarthestPointInDirection(-direction, body2.v);
//	s.minkowskiDifference = s.pointA - s.pointB;
//
//	return s;
//}
//
//glm::vec3 getFarthestPointInDirection(glm::vec3 direction, const std::vector<glm::vec3>& vertices)
//{
//	float maxDot = glm::dot(vertices[0],direction);
//
//	int indexDot = 0;
//	float currentDot;
//	for (int i = 1; i < vertices.size(); i++)
//	{
//		currentDot = glm::dot(direction,vertices[i]);
//		if (currentDot > maxDot){
//			maxDot = currentDot;
//			indexDot = i;
//		}
//	}
//
//	return vertices[indexDot];
//}
//
//bool processSimplex(std::vector<Simplex> &simplex, glm::vec3 &direction)
//{
//	Simplex A,B,C,D;
//	glm::vec3 AB,AC,AD,AO;
//
//	switch(simplex.size())
//	{
//	case 2:
//
//		A = simplex.at(1);
//		B = simplex.at(0);
//
//		AB = B.minkowskiDifference - A.minkowskiDifference;
//		AO = -A.minkowskiDifference;
//
//
//		if(isSameDirection(AO,AB))
//		{
//			direction = glm::cross(glm::cross(AB, AO), AB);
//		}
//		else
//		{
//			direction = AO;
//		}
//
//		return false;
//	case 3:
//
//		return checkTriangle(simplex, direction);
//	}
//}
//
//bool isSameDirection(glm::vec3 &a, glm::vec3 &b)
//{
//	float dot = glm::dot(a, b);
//	return dot > 0.0f;
//}
//
//bool checkTriangle(std::vector<Simplex> &simplex, glm::vec3 &direction)
//{
//	Simplex A,B,C;
//	glm::vec3 AB,AC,AO;
//
//	A = simplex[2];
//	B = simplex[1];
//	C = simplex[0];
//
//	AB = B.minkowskiDifference - A.minkowskiDifference;
//	AC = C.minkowskiDifference - A.minkowskiDifference;
//	AO = -A.minkowskiDifference;
//
//	glm::vec3 ABC = glm::cross(AB, AC);
//
//	if(isSameDirection(glm::cross(ABC, AC), AO)) // AC plane 
//	{
//		if(isSameDirection(AC, AO)) // outside AC edge
//		{
//			direction = glm::cross(glm::cross(AC, AO), AC);
//			simplex.erase(simplex.begin() + 1);
//		}
//		else
//		{
//			if(isSameDirection(AB, AO)) // outside AB edge
//			{
//				direction = glm::cross(glm::cross(AB, AO), AB);
//				simplex.erase(simplex.begin());
//			}
//			else // outside A
//			{
//				direction = AO;
//				simplex.erase(simplex.begin());
//				simplex.erase(simplex.begin());
//			}
//		}
//	}
//	else // inside AC 
//	{
//		if(isSameDirection(glm::cross(AB, ABC), AO)) // AB plane 
//		{
//			if(isSameDirection(AB, AO)) // outside AB plane
//			{
//				direction = glm::cross(glm::cross(AB, AO), AB);
//				simplex.erase(simplex.begin());
//			}
//			else // outside A
//			{
//				direction = AO;
//				simplex.erase(simplex.begin());
//				simplex.erase(simplex.begin());
//			}
//		}
//		else // orthogonal to face
//		{
//			if(isSameDirection(ABC, AO)) // outside face
//			{
//				direction = ABC;
//			}
//			else // inside face
//			{
//				simplex[0] = B;
//				simplex[1] = C;
//
//				direction = -ABC;
//			}
//		}
//	}
//	return false;
//}