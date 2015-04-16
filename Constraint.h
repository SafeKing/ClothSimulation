#pragma once
#include "Particle.h"

class Constraint
{
public:
	Constraint(Particle *p1, Particle *p2);
	~Constraint(void);

	bool unbearFlag;

	Particle *p1, *p2; 
	void set_stiffness(float k);
	void satisfyConstraint();
	bool unbearable();

protected:
	float m_stiffness;

private:

	float rest_distance;
};

