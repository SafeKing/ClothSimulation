#pragma once
#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <glm/gtc/type_ptr.hpp>
#include "Particle.h"
#include "Constraint.h"
#include "controls.hpp"
#include "Shader.hpp"
#include <SOIL.h>
#include "texture.hpp"
#include "collisionDetector.h"

#define DAMPING 0.01f // how much to damp the cloth simulation each frame
#define TIME_STEPSIZE_SQUARE 0.5f*0.5f 
#define CONSTRAINT_ITERATIONS 30 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)
#define SMALL_NUM   0.0001
using namespace glm;

struct Simplex{
	vec3 pointA;
	vec3 pointB;
	vec3 minkowskiDifference;
};

class cloth
{
public:

	cloth::cloth(float width, float height, int num_particles_width, int num_particles_height);
	~cloth(void);

	struct Vertex {
		vec3 position;
		vec2 uv;
		vec3 normal;
	};

	struct Face {
		Particle* particleA;
		Particle* particleB;
		Particle* particleC;
		vec3 centerPos;
		vec3 normal;
	};

	struct Ray {
		vec3 P0;//old position
		vec3 P1;//predict position
		vec3 dir;
	}; 

	struct ParticleConstraint {
		int otherParticleX;
		int otherParticleY;
		float restPosition;
	};

	

	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	
	std::vector<Particle> particles; // all particles that are part of this cloth
	std::vector<Particle> particlesAlt; // all particles that are part of this cloth	

	std::vector<Particle> *particlesCurrent; // pointer to the current particles
	std::vector<Particle*> fixedParticles; // pointer to the current particles

	std::vector<Constraint> constraints; // alle constraints between particles as part of this cloth
	std::vector<std::vector<ParticleConstraint>> constrainsPerParticle; 

	int intersect3D_RayTriangle( Ray R, Face T );
	
	GLuint vertexArrayObject;
	GLuint vertexBuffer;
	GLuint texture;
	int elementSize;

	bool collisionFlag;
	float clothThick;
	bool draw_wire;
	int getParticleIndex(int x, int y);
	Particle* getParticle(int x, int y);
	vec3 calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3);
	void addWindForcesForTriangle(Particle *p1,Particle *p2,Particle *p3, const vec3 direction);
	void render(Shader* shader);
	void updatePos(float dt);
	void addForce(const vec3 direction);
	void windForce(const vec3 direction);
	bool ballCollision(const vec3 center,const float radius );
	void createParticles(float width, float height, int num_particles_x, int num_particles_y);
	void createSprings();
	void initParticlePairConstraint(int x1, int y1, int x2, int y2);
	void setFixedPoints(int x, int y);
	void updateFaceNormal();
	void SaveParticleVertexInfo(Particle *p1, const vec2 uv, std::vector<Vertex> &vertexData);
	 GLuint loadClothTexture(const char* path);
	 GLuint clothTextureID;
	 GLuint iterativeTimes;

	 void collisionDetection(vec3 center, float radius);
	 void generateFace(Particle *p1,Particle *p2,Particle *p3);
	 bool testTriangleIntersect(Particle* currentParticle);
	 void selfCollision();
	 collisionDetector* collisionChecker;
	 std::vector<Face> faces;
	 bool planeCollision();
	/* bool NarrowPhaseCheck(Face faceA, Face faceB);
	 vec3 supportFunction(Face faceA, Face faceB, vec3 &direction) */
	 //bool DoTriangleSimplex(Face face, glm::vec3 &direction);
	 bool IntersectTriangle(Ray ray, Face face);
	 bool PointinTriangle(vec3 A, vec3 B, vec3 C, vec3 P);
	 vec3 intersectQ;

	 bool NarrowPhaseCheck(Face &body1, Face &body2);
	 Simplex support(glm::vec3 direction, Face &body1, Face &body2);
	 glm::vec3 getFarthestPointInDirection(glm::vec3 direction, const std::vector<glm::vec3>& vertices);
	 bool processSimplex(std::vector<Simplex> &simplex, glm::vec3 &direction);
	 bool isSameDirection(glm::vec3 &a, glm::vec3 &b);
	 bool checkTriangle(std::vector<Simplex> &simplex, glm::vec3 &direction);
	 std::vector<Simplex> simplex;
};

