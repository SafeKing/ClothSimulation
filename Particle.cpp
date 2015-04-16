#include "Particle.h"

#define NUM_PARTICLES_WIDTH 48
#define NUM_PARTICLES_HEIGHT 48

#define DAMPING 0.01f // how much to damp the cloth simulation each frame
#define CONSTRAINT_ITERATIONS 15 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)
Particle::Particle(void)
{
}

Particle::Particle(vec3 pos) : pos(pos), old_pos(pos),acceleration(vec3(0,0,0)), mass(1), movable(true), accumulated_normal(vec3(0,0,0))
{
	fixedFlag = false;
	particleIndex= 0;
}


Particle::~Particle(void)
{
}

void Particle::addForce(vec3 f)
{
	acceleration += f/mass;
}

void Particle::compute_predicted_position(float dt)
{
	if(movable)
	{
		vec3 temp = pos;
		pos = pos + (pos-old_pos)*(1.0f-DAMPING) + acceleration*dt;
		old_pos = temp;
		acceleration = vec3(0,0,0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)	
	}
}

vec3& Particle::getPos() 
{
	return pos;
}

void Particle::resetAcceleration() 
{
	acceleration = vec3(0,0,0);
}

void Particle::setFixed() 
{
	fixedFlag = true;
}

void Particle::offsetPos(const vec3 v)
{ 
	if(movable) pos += v;
}

void Particle::setPos( vec3 setPosition)
{ 
	if(movable) pos = setPosition;
}

void Particle::makeUnmovable() 
{
	movable = false;
}

void Particle::addToNormal(vec3 normal)
{
	accumulated_normal += normalize(normal);
}

vec3& Particle::getNormal() 
{ 
	return accumulated_normal;
} // notice, the normal is not unit length

void Particle::resetNormal() 
{
	accumulated_normal = vec3(0,0,0);
}