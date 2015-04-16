#include "Constraint.h"


Constraint::Constraint(Particle *p1, Particle *p2) :  p1(p1),p2(p2)
{
	vec3 vec = p1->getPos()-p2->getPos();
	rest_distance = length(vec);
	unbearFlag = false;
	m_stiffness = 1;
}


Constraint::~Constraint(void)
{
}

void Constraint::satisfyConstraint()
{
	vec3 p1_to_p2 = p2->getPos()-p1->getPos(); // vector from p1 to p2
	float current_distance = length(p1_to_p2); // current distance between p1 and p2
	vec3 correctionVector = p1_to_p2*(m_stiffness - rest_distance/current_distance); // The offset vector that could moves p1 into a distance of rest_distance to p2
	vec3 correctionVectorHalf = correctionVector*0.5f; // Lets make it half that length, so that we can move BOTH p1 and p2.
	p1->offsetPos(correctionVectorHalf); // correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
	p2->offsetPos(-correctionVectorHalf); // we must move p2 the negative direction of correctionVectorHalf since it points from p2 to p1, and not p1 to p2.	
}

bool Constraint::unbearable()
{
	unbearFlag = false;
	vec3 p1_to_p2 = p2->getPos()-p1->getPos(); // vector from p1 to p2
	float current_distance = length(p1_to_p2); // current distance between p1 and p2
	if (current_distance > 1.2f * rest_distance)
	{
		unbearFlag = true;
	}
	return unbearFlag;
}

void Constraint::set_stiffness(float k)
{
	m_stiffness = k;
}