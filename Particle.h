#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Particle
{
public:
	Particle(void);
	Particle(vec3 pos);
	~Particle(void);

	void addForce(vec3 f);
	void compute_predicted_position(float dt);
	vec3& getPos();
	void resetAcceleration();
	void offsetPos(const vec3 v);
	void setPos( vec3 setPosition);
	void makeUnmovable();
	void addToNormal(vec3 normal);
	vec3& getNormal();
	void resetNormal();
	void setFixed();
	bool movable;
	bool fixedFlag;
	vec3 pos; // the current position of the particle in 3D space
	vec3 old_pos;
	bool selfCollisionFlag;
	int particleIndex;

	
private:
	//bool movable; // can the particle move or not ? used to pin parts of the cloth

	float mass; // the mass of the particle (is always 1 in this example)
	//vec3 pos; // the current position of the particle in 3D space
	//vec3 old_pos; // the position of the particle in the previous time step, used as part of the verlet numerical integration scheme
	vec3 acceleration; // a vector representing the current acceleration of the particle
	vec3 accumulated_normal; // an accumulated normal (i.e. non normalized), used for OpenGL soft shading

};

