#include "cloth.h"

cloth::cloth(float width, float height, int num_particles_x, int num_particles_y) : num_particles_width(num_particles_x), num_particles_height(num_particles_y)
{
	faces.clear();
	fixedParticles.clear();
	particles.clear();
	constraints.clear();

	createParticles( width, height, num_particles_x, num_particles_y);
	createSprings();
	
	collisionFlag = false;
	draw_wire = false;
	clothThick = 0.1;
	collisionChecker = new collisionDetector();
	intersectQ=vec3(0,0,0);
	iterativeTimes = 15;
}

void cloth::setFixedPoints(int x, int y)
{
	getParticle(x ,y)->makeUnmovable(); 
	getParticle(x ,y)->setFixed();
	fixedParticles.push_back(getParticle(x ,y));
	
}

cloth::~cloth(void)
{
}

void cloth::createParticles(float width, float height, int num_particles_x, int num_particles_y)
{
	
	num_particles_width = num_particles_x; 
	num_particles_height = num_particles_y;
	particles.resize(num_particles_width*num_particles_height); 

	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for(int x=0; x<num_particles_width; x++)
	{
		for(int y=0; y<num_particles_height; y++)
		{
			float unitLength_x = width/(float)num_particles_width;
			float unitLength_y = height/(float)num_particles_height;
			vec3 particlePos = vec3(x * unitLength_x, -y * unitLength_y, 0);
			//Particle temp = Particle(particlePos);
			particles[y*num_particles_width+x] = Particle(particlePos); //（x,y）处的粒子在第y行x列
			particles[y*num_particles_width+x].particleIndex = y*num_particles_width+x;
			//printf("  %f particle position X\n", particles[y*num_particles_width+x].position.x);
			//printf("  %f particle position Y\n", particles[y*num_particles_width+x].position.y);
			constrainsPerParticle.push_back(std::vector<ParticleConstraint>()); //作用在particle上的限制力
		}
	}
	for(int x = 0; x<num_particles_width-1; x++)
	{
		for(int y=0; y<num_particles_height-1; y++)
		{
			generateFace(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1));
			generateFace(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1));
		}
	}
	printf("faces number is %d \n",faces.size());
}

void cloth::createSprings()
{
	for(int x=0; x<num_particles_width; x++)
	{
		for(int y=0; y<num_particles_height; y++)
		{
			//structural spring
			if (x<num_particles_width-1) initParticlePairConstraint(x,y ,x+1,y);//structure spring between particle(x,y)and(x+1,y)
			if (y<num_particles_height-1) initParticlePairConstraint(x,y,x,y+1);//粒子（x,y）和（x,y+1)之间的结构弹簧
			//shear spring
			if (x<num_particles_width-1 && y<num_particles_height-1) initParticlePairConstraint(x,y,x+1,y+1);//shear spring between particle(x,y)and(x+1,y+1)
			if (x<num_particles_width-1 && y<num_particles_height-1) initParticlePairConstraint(x+1,y,x,y+1);//粒子（x+1,y）和（x,y+1)之间的剪切弹簧
			//blend spring
			if (x<num_particles_width-2) initParticlePairConstraint(x,y,x+2,y);
			if (y<num_particles_height-2) initParticlePairConstraint(x,y,x,y+2);
			if (x<num_particles_width-2 && y<num_particles_height-2) initParticlePairConstraint(x,y,x+2,y+2);
			if (x<num_particles_width-2 && y<num_particles_height-2) initParticlePairConstraint(x+2,y,x,y+2);	
		}
	}

}
void cloth::initParticlePairConstraint(int x1, int y1, int x2, int y2) {
	Particle *p1 = getParticle(x1,y1);
	Particle *p2 = getParticle(x2,y2);

	constraints.push_back( Constraint(p1,p2));//p1,p2,及弹簧之间的自然长度存为一个结构体

	vec3 vec = p1->getPos()-p2->getPos();
	float rest_distance = length(vec);

	ParticleConstraint pc1 = {x2,y2,rest_distance};//p1点受到p2点的constraint
	constrainsPerParticle[getParticleIndex(x1,y1)].push_back(pc1);//将p1收到的p2的constraint压入p1的constraint集合

	ParticleConstraint pc2 = {x1,y1,rest_distance};//p2点受到p1点的constraint
	constrainsPerParticle[getParticleIndex(x2,y2)].push_back(pc2);//将p2收到的p1的constraint压入p1的constraint集合
}

int cloth::getParticleIndex(int x, int y)
{
	return y*num_particles_width + x; 
}

Particle* cloth::getParticle(int x, int y) 
{
	return &particles[getParticleIndex(x,y)];
}

vec3 cloth::calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3)
{
	vec3 pos1 = p1->getPos();//A
	vec3 pos2 = p2->getPos();//B
	vec3 pos3 = p3->getPos();//C

	vec3 v1 = pos2-pos1;//B-A
	vec3 v2 = pos3-pos1;//C-A

	return cross(v1,v2);
}

void cloth::addWindForcesForTriangle(Particle *p1,Particle *p2,Particle *p3, const vec3 direction)
{
	vec3 normal = calcTriangleNormal(p1,p2,p3);
	vec3 d = normalize(normal);
	vec3 force = normal*(dot(d,direction));
	p1->addForce(force);
	p2->addForce(force);
	p3->addForce(force);
}

void cloth::generateFace(Particle *p1,Particle *p2,Particle *p3)
{
	Face face;
	face.particleA = p1;
	face.particleB = p2;
	face.particleC = p3;
	face.normal = normalize(calcTriangleNormal(p1,p2,p3));
	face.centerPos.x =(p1->getPos().x+p2->getPos().x+p3->getPos().x)/3;
	face.centerPos.y =(p1->getPos().y+p2->getPos().y+p3->getPos().y)/3;
	face.centerPos.z =(p1->getPos().z+p2->getPos().z+p3->getPos().z)/3;
	faces.push_back(face);
}

//bool cloth::testTriangleIntersect(Particle* currentParticle)
//{
//	bool testResult = false;
//	//test each particles, see if it's collide with the plane(face)
//	//plane equation: ax+by+cz=d, n=(a,b,c)
//	//ray equation: r = p+t*dirc;
//	for (int i = 0; i<faces.size(); i++)
//	{
//		Face currentface = faces[i];
//		faces[i].normal = normalize(calcTriangleNormal(faces[i].particleA,faces[i].particleB,faces[i].particleC));
//		glm::vec3 n = faces[i].normal;
//		
//		glm::vec3 p = currentParticle->pos;
//		glm::vec3 rayDirection = p - currentParticle->old_pos;
//		float nDotD = glm::dot(rayDirection, n);
//		if (abs(nDotD) <= 0.0001)
//		{
//			return false;
//		}
//		else
//		{
//			float d = glm::dot(n, faces[i].particleA->getPos());
//			//find out the intersect point
//			float t = (d-glm::dot(n,p))/(nDotD);
//			intersectQ = p + t*rayDirection;
//			/*if ((abs(intersectQ.x)>max(p.x,currentParticle.old_pos.x)) || (abs(intersectQ.y)>max(p.y,currentParticle.old_pos.y)) || (abs(intersectQ.z)>max(p.z,currentParticle.old_pos.z)))
//			{
//				return false;
//			}*/
//			//test if Q inside line AB
//			glm::vec3 AB = faces[i].particleB->getPos() - faces[i].particleA->getPos();
//			glm::vec3 AQ = intersectQ - faces[i].particleA->getPos();
//			glm::vec3 testAB = glm::cross(AB,AQ);
//			//test if Q inside line BC
//			glm::vec3 BC = faces[i].particleC->getPos() - faces[i].particleB->getPos();
//			glm::vec3 BQ = intersectQ - faces[i].particleB->getPos();
//			glm::vec3 testBC = glm::cross(BC,BQ);
//			//test if Q inside line AC
//			glm::vec3 CA = faces[i].particleA->getPos() - faces[i].particleC->getPos();
//			glm::vec3 CQ = intersectQ - faces[i].particleC->getPos();
//			glm::vec3 testAC = glm::cross(CA,CQ);
//			if (glm::dot(testAB,n)>=0 && glm::dot(testBC,n)>=0 && glm::dot(testAC,n)>=0)
//			{
//				
//				/*if (glm::dot(n,p- faces[i].particleA->getPos())*glm::dot(n,currentParticle->old_pos- faces[i].particleA->getPos())<0)
//				{
//					return true;
//				}*/
//				return true;
//			}
//	///////////////////////////////////////////////		////////////////////////////////////////////////////////////////
//			/*if (PointinTriangle(faces[i].particleA->getPos(),faces[i].particleB->getPos(),faces[i].particleC->getPos(),intersectQ))
//			{
//				return true;
//			}*/
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//			//vec3 PQ = currentParticle.old_pos-faces[i].particleA->getPos();
//			//float dist = abs(dot(PQ,n));
//			//if (dist>abs(nDotD) && nDotD<0)
//			//{
//			//	float d = glm::dot(n, faces[i].particleA->getPos());
//			//	//find out the intersect point
//			//	float t = (d-glm::dot(n,p))/(nDotD);
//			//	intersectQ = p + t*rayDirection;
//			//	//test if Q inside line AB
//			//	glm::vec3 AB = faces[i].particleB->getPos() - faces[i].particleA->getPos();
//			//	glm::vec3 AQ = intersectQ - faces[i].particleA->getPos();
//			//	glm::vec3 testAB = glm::cross(AB,AQ);
//			//	//test if Q inside line BC
//			//	glm::vec3 BC = faces[i].particleC->getPos() - faces[i].particleB->getPos();
//			//	glm::vec3 BQ = intersectQ - faces[i].particleB->getPos();
//			//	glm::vec3 testBC = glm::cross(BC,BQ);
//			//	//test if Q inside line AC
//			//	glm::vec3 CA = faces[i].particleA->getPos() - faces[i].particleC->getPos();
//			//	glm::vec3 CQ = intersectQ - faces[i].particleC->getPos();
//			//	glm::vec3 testAC = glm::cross(CA,CQ);
//			//	if (glm::dot(testAB,n)>=0 && glm::dot(testBC,n)>=0 && glm::dot(testAC,n)>=0)
//			//	{
//			//		return true;
//			//	}
//			//}
//			return false;
//		}
//		/*Ray ray;
//		ray.P1 = currentParticle.getPos();//predict pos
//		ray.P0 = currentParticle.old_pos;//old pos
//		ray.dir = currentParticle.getPos()-currentParticle.old_pos;
//		int testResult = intersect3D_RayTriangle(ray,faces[i]);
//		if (testResult==1)
//		{
//			return true;
//		}else{
//			return false;
//		}*/
//
//		/*if (IntersectTriangle(ray,faces[i]))
//		{
//			return true;
//		}*/
//		
//	}
//	//return false;
//}

bool cloth::testTriangleIntersect(Particle* currentParticle)
{
	bool testResult = false;
	//test each particles, see if it's collide with the plane(face)
	//plane equation: ax+by+cz=d, n=(a,b,c)
	//ray equation: r = p+t*dirc;
	for (int i = 0; i<faces.size(); i++)
	{
		if (faces[i].particleA->particleIndex != currentParticle->particleIndex && faces[i].particleB->particleIndex != currentParticle->particleIndex && faces[i].particleC->particleIndex != currentParticle->particleIndex)
		{
			Face currentface = faces[i];
			glm::vec3 n = normalize(calcTriangleNormal(faces[i].particleA,faces[i].particleB,faces[i].particleC));

			//glm::vec3 p = currentParticle->pos;
			glm::vec3 rayDirection = currentParticle->pos - currentParticle->old_pos;
			float nDotD = glm::dot(rayDirection, n);
			if (abs(nDotD) <= 0.00001 || nDotD ==0)
			{
				return false;
			}
			else
			{
				float d = glm::dot(n, faces[i].particleA->getPos());
				//float d = n.x * faces[i].particleA->getPos().x+n.y * faces[i].particleA->getPos().y+n.z * faces[i].particleA->getPos().z;
				//find out the intersect point
				float t = (d-glm::dot(n,currentParticle->old_pos))/nDotD;
				//float t = dot(n,(faces[i].particleA->getPos()-currentParticle->old_pos))/(nDotD);
				intersectQ = currentParticle->old_pos + t*rayDirection;
				
				if (((intersectQ.x)>=max(currentParticle->pos.x,currentParticle->old_pos.x)) || ((intersectQ.y)>=max(currentParticle->pos.y,currentParticle->old_pos.y)) || ((intersectQ.z)>=max(currentParticle->pos.z,currentParticle->old_pos.z)) ||
					((intersectQ.x)<=min(currentParticle->pos.x,currentParticle->old_pos.x)) || ((intersectQ.y)<=min(currentParticle->pos.y,currentParticle->old_pos.y)) || ((intersectQ.z)<=min(currentParticle->pos.z,currentParticle->old_pos.z))
				   )
				{
					return false;
				}
				////test if Q inside line AB
				//glm::vec3 AB = faces[i].particleB->getPos() - faces[i].particleA->getPos();
				//glm::vec3 AQ = intersectQ - faces[i].particleA->getPos();
				//glm::vec3 testAB = glm::cross(AB,AQ);
				////test if Q inside line BC
				//glm::vec3 BC = faces[i].particleC->getPos() - faces[i].particleB->getPos();
				//glm::vec3 BQ = intersectQ - faces[i].particleB->getPos();
				//glm::vec3 testBC = glm::cross(BC,BQ);
				////test if Q inside line AC
				//glm::vec3 CA = faces[i].particleA->getPos() - faces[i].particleC->getPos();
				//glm::vec3 CQ = intersectQ - faces[i].particleC->getPos();
				//glm::vec3 testAC = glm::cross(CA,CQ);
				/*if (glm::dot(testAB,n)>=0 && glm::dot(testBC,n)>=0 && glm::dot(testAC,n)>=0)
				{
					
					return true;
				}*/
				if(PointinTriangle(faces[i].particleA->getPos(),faces[i].particleB->getPos(),faces[i].particleC->getPos(),intersectQ))
				{
					currentParticle->pos = currentParticle->old_pos; 
					printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
					return true;
					
				}
			}	
		}
	}
	return false;
}

//bool cloth::testTriangleIntersect(Particle* currentParticle)
//{
//	bool testResult = false;
//	//test each particles, see if it's collide with the plane(face)
//	//plane equation: ax+by+cz=d, n=(a,b,c)
//	//ray equation: r = p+t*dirc;
//	for (int i = 0; i<faces.size(); i++)
//	{
//		if (faces[i].particleA->particleIndex != currentParticle->particleIndex && faces[i].particleA->particleIndex != currentParticle->particleIndex && faces[i].particleA->particleIndex != currentParticle->particleIndex)
//		{
//			if (PointinTriangle(faces[i].particleA->getPos(),faces[i].particleB->getPos(),faces[i].particleC->getPos(),currentParticle->getPos()))
//			{
//				printf("********************************************");
//				return true;
//			}
//		}
//		
//	}
//	return false;
//}
//bool cloth::PointinTriangle(vec3 A, vec3 B, vec3 C, vec3 P)
//{
//	vec3 v0 = C - A ;
//	vec3 v1 = B - A ;
//	vec3 v2 = P - A ;
//
//	float dot00 = dot(v0,v0) ;
//	float dot01 = dot( v0,v1) ;
//	float dot02 = dot(v0,v2) ;
//	float dot11 = dot(v1,v1) ;
//	float dot12 = dot(v1,v2) ;
//
//	float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01) ;
//
//	float u = (dot11 * dot02 - dot01 * dot12) * inverDeno ;
//	if (u < 0 || u > 1) // if u out of range, return directly
//	{
//		return false ;
//	}
//
//	float v = (dot00 * dot12 - dot01 * dot02) * inverDeno ;
//	if (v < 0 || v > 1) // if v out of range, return directly
//	{
//		return false ;
//	}
//
//	return u + v <= 1 ;
//}

//////////////////////////////////////////////////////////////////////////////
//THIS BARYCENTRIC CODE COMES FROM TUTORIAL SHOWN AS BELOW
//http://blogs.msdn.com/b/rezanour/archive/2011/08/07/barycentric-coordinates-and-point-in-triangle-tests.aspx
bool cloth::PointinTriangle(vec3 A, vec3 B, vec3 C, vec3 P)
{
	// Prepare our barycentric variables
	glm::vec3 u = B - A;
	glm::vec3 v = C - A;
	glm::vec3 w = P - A;

	glm::vec3 vCrossW = glm::cross(v, w);
	glm::vec3 vCrossU = glm::cross(v, u);

	// Test sign of r
	if (glm::dot(vCrossW, vCrossU) < 0)
		return false;

	glm::vec3 uCrossW = glm::cross(u, w);
	glm::vec3 uCrossV = glm::cross(u, v);

	// Test sign of t
	if (glm::dot(uCrossW, uCrossV) < 0)
		return false;

	// At this point, we know that r and t and both > 0.
	// Therefore, as long as their sum is <= 1, each must be less <= 1
	float denom = glm::length(uCrossV);
	float r = glm::length(vCrossW) / denom;
	float t = glm::length(uCrossW) / denom;


	return (r + t <= 1);
}

int cloth::intersect3D_RayTriangle( Ray R, Face T )
{
	vec3    u, v, n;              // triangle vectors
	vec3    dir, w0, w;           // ray vectors
	float     r, a, b;              // params to calc ray-plane intersect

	u = T.particleB->getPos() - T.particleA->getPos();
	v = T.particleC->getPos() - T.particleA->getPos();
	n = cross(u,v);              // cross product
	// get triangle edge vectors and plane normal
	if (n == (vec3)0)             // triangle is degenerate
		return -1;                  // do not deal with this case

	dir = R.P1 - R.P0;              // ray direction vector
	w0 = R.P0 - T.particleA->getPos();
	a = -dot(n,w0);
	b = dot(n,dir);
	if (fabs(b) < SMALL_NUM) {     // ray is  parallel to triangle plane
		if (a <= 0.001)                 // ray lies in triangle plane
			return 2;
		else return 0;              // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0)                    // ray goes away from triangle
		return 0;                   // => no intersect
	// for a segment, also test if (r > 1.0) => no intersect

	vec3 I;
	I = R.P0 + r * dir;            // intersect point of ray and plane

	// is I inside T?
	float    uu, uv, vv, wu, wv, D;
	uu = dot(u,u);
	uv = dot(u,v);
	vv = dot(v,v);
	w = I - T.particleA->getPos();
	wu = dot(w,u);
	wv = dot(w,v);
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0|| s > 1)         // I is outside T
		return 0;
	t = (uv * wu - uu * wv) / D;
	if (t < 0 || (s + t) > 1)  // I is outside T
		return 0;
	intersectQ = I;
	return 1;                       // I is in T
}

//bool IntersectTriangle(const vec3& orig, const vec3& dir,
//					   vec3& v0, vec3& v1, vec3& v2,
//					   float* t, float* u, float* v)
bool cloth::IntersectTriangle(Ray ray,Face face)
{
	/*float* t = &intersectQ[0];
	float* u= &intersectQ[1];
	float* v =&intersectQ[2];*/
	//vec3 orig = ray.P0;
	//vec3 v0 = face.particleA->getPos();
	//vec3 v1 = face.particleB->getPos();
	//vec3 v2 = face.particleC->getPos();
	//vec3 dir = ray.dir;
	// E1
	vec3 E1 = face.particleB->getPos() - face.particleA->getPos();

	// E2
	vec3 E2 = face.particleC->getPos() - face.particleA->getPos();

	// P
	//vec3 P = dir.Cross(E2);
	vec3 P = cross(ray.dir,E2);
	// determinant
	//float det = E1.Dot(P);
	float det = dot(E1,P);
	// keep det > 0, modify T accordingly
	vec3 T;
	if( det >0 )
	{
		T = ray.P0 - face.particleA->getPos();
	}
	else
	{
		T = face.particleA->getPos() - ray.P0;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if( det < 0.01f )
		return false;

	// Calculate u and make sure u <= 1
	//*u = T.Dot(P);
	float u = dot(T,P);
	if( u < 0.0f || u > det )
		return false;

	// Q
	//vec3 Q = T.Cross(E1);
	vec3 Q = cross(T,E1);
	// Calculate v and make sure u + v <= 1
	//*v = dir.Dot(Q);
	float v = dot(ray.dir,Q);
	if( v < 0.0f || u + v > det )
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	//*t = dot(E2,Q);
	//float fInvDet = 1.0f / det;
	//*t *= fInvDet;
	//*u *= fInvDet;
	//*v *= fInvDet;
	//intersectQ = vec3(*t, *u, *v);
	return true;
}

void cloth::selfCollision()
{
	bool t =false;
	
	//for (int i = 0; i<faces.size(); i++)
	//{
	//	for (int j = 0; j<faces.size(); j++)
	//	{
	//		if (faces[i].particleA->particleIndex != faces[j].particleA->particleIndex && faces[i].particleA->particleIndex != faces[j].particleB->particleIndex && faces[i].particleA->particleIndex != faces[j].particleC->particleIndex &&
	//			faces[i].particleB->particleIndex != faces[j].particleA->particleIndex && faces[i].particleB->particleIndex != faces[j].particleB->particleIndex && faces[i].particleB->particleIndex != faces[j].particleC->particleIndex && 
	//			faces[i].particleC->particleIndex != faces[j].particleA->particleIndex && faces[i].particleC->particleIndex != faces[j].particleB->particleIndex && faces[i].particleC->particleIndex != faces[j].particleC->particleIndex)
	//		{
	//			if (NarrowPhaseCheck(faces[i],faces[j]))
	//			{
	//				/*faces[i].p1->pos = faces[i].p1->oldPosition;
	//				faces[i].p2->pos = faces[i].p2->oldPosition;
	//				faces[i].p3->pos = faces[i].p3->faces;*/

	//				printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	//			}
	//		}
	//	}
	//}
	//printf("faces number is %d \n",faces.size());
	bool flag = true;
	for(int i = 0; i<particles.size(); i++)
	{
		
		
		Particle* p = &particles[i];
	    for (int i = 0; i<faces.size(); i++)
		{
			if (faces[i].particleA->particleIndex != p->particleIndex && faces[i].particleB->particleIndex != p->particleIndex && faces[i].particleC->particleIndex != p->particleIndex)
			{
				if (PointinTriangle(faces[i].particleA->pos,faces[i].particleB->pos,faces[i].particleC->pos,p->pos))
				{
					selfCollisionResponse(faces[i],p);
						//p->pos = p->old_pos;
					flag = false;
				}
			}
		}
		if (flag)
		{
			testTriangleIntersect(p);
		}
	}

	/*for(int i = 0; i<particles.size(); i++)
	{
		Particle* currentParticle = &particles[i];
		for (int j = 0; j<faces.size(); j++)
		{
			if (faces[j].particleA->particleIndex != currentParticle->particleIndex && faces[j].particleB->particleIndex != currentParticle->particleIndex && faces[j].particleC->particleIndex != currentParticle->particleIndex)
			{
				if (PointinTriangle(faces[i].particleA->getPos(),faces[i].particleB->getPos(),faces[i].particleC->getPos(),currentParticle->getPos()))
				{
					printf("********************************************");
				
				}
			}

		}
		
	}*/
	
	//if (!flag){
	//	std::vector<Constraint>::iterator constraint;
	//	for(int i=0; i<15; i++) // iterate over all constraints several times
	//	{
	//		for(constraint = constraints.begin(); constraint != constraints.end(); constraint++ )
	//		{
	//			(*constraint).satisfyConstraint(); // satisfy constraint.
	//		}
	//	}
	//}

	
}

bool cloth::NarrowPhaseCheck(Face &body1, Face &body2)
{
	//demoResult.clear();
	//std::vector<Simplex> simplex;

	glm::vec3 direction = body1.particleA->getPos() - body2.particleA->getPos();

	simplex.push_back(support(direction, body1, body2));

	direction = -simplex[0].minkowskiDifference;
	int counter = 100;

	while (counter > 0)
	{
		Simplex tempSimplex;
		tempSimplex = support(direction, body1, body2);

		// Last point added was not past the origin in this direction
		if(glm::dot(tempSimplex.minkowskiDifference, direction) < 0)
		{
			return false;
		}
		simplex.push_back(tempSimplex);

		//check intersect
		if (processSimplex(simplex, direction))
		{
			return true;
		}
		counter--;
	}
}

Simplex cloth::support(glm::vec3 direction, Face &body1, Face &body2)
{
	Simplex s;
	std::vector<glm::vec3> v1;
	v1.push_back(body1.particleA->getPos());
	v1.push_back(body1.particleB->getPos());
	v1.push_back(body1.particleC->getPos());
	s.pointA = getFarthestPointInDirection(direction, v1);
	std::vector<glm::vec3> v2;
	v2.push_back(body1.particleA->getPos());
	v2.push_back(body1.particleB->getPos());
	v2.push_back(body1.particleC->getPos());
	s.pointB = getFarthestPointInDirection(-direction, v2);
	s.minkowskiDifference = s.pointA - s.pointB;

	/*v1.clear();
	v2.clear();*/
	return s;
}

glm::vec3 cloth::getFarthestPointInDirection(glm::vec3 direction, const std::vector<glm::vec3>& vertices)
{
	float maxDot = glm::dot(vertices[0],direction);

	int indexDot = 0;
	float currentDot;
	for (int i = 1; i < vertices.size(); i++)
	{
		currentDot = glm::dot(direction,vertices[i]);
		if (currentDot > maxDot){
			maxDot = currentDot;
			indexDot = i;
		}
	}

	return vertices[indexDot];
}

bool cloth::processSimplex(std::vector<Simplex> &simplex, glm::vec3 &direction)
{
	Simplex A,B,C,D;
	glm::vec3 AB,AC,AD,AO;

	switch(simplex.size())
	{
	case 2:

		A = simplex.at(1);
		B = simplex.at(0);

		AB = B.minkowskiDifference - A.minkowskiDifference;
		AO = -A.minkowskiDifference;


		if(isSameDirection(AO,AB))
		{
			direction = glm::cross(glm::cross(AB, AO), AB);
		}
		else
		{
			direction = AO;
		}

		return false;
	case 3:

		return checkTriangle(simplex, direction);
	}
}

bool cloth::isSameDirection(glm::vec3 &a, glm::vec3 &b)
{
	float dot = glm::dot(a, b);
	return dot > 0.0f;
}

bool cloth::checkTriangle(std::vector<Simplex> &simplex, glm::vec3 &direction)
{
	Simplex A,B,C;
	glm::vec3 AB,AC,AO;

	A = simplex[2];
	B = simplex[1];
	C = simplex[0];

	AB = B.minkowskiDifference - A.minkowskiDifference;
	AC = C.minkowskiDifference - A.minkowskiDifference;
	AO = -A.minkowskiDifference;

	glm::vec3 ABC = glm::cross(AB, AC);

	if(isSameDirection(glm::cross(ABC, AC), AO)) // AC plane 
	{
		if(isSameDirection(AC, AO)) // outside AC edge
		{
			direction = glm::cross(glm::cross(AC, AO), AC);
			simplex.erase(simplex.begin() + 1);
		}
		else
		{
			if(isSameDirection(AB, AO)) // outside AB edge
			{
				direction = glm::cross(glm::cross(AB, AO), AB);
				simplex.erase(simplex.begin());
			}
			else // outside A
			{
				direction = AO;
				simplex.erase(simplex.begin());
				simplex.erase(simplex.begin());
			}
		}
	}
	else // inside AC 
	{
		if(isSameDirection(glm::cross(AB, ABC), AO)) // AB plane 
		{
			if(isSameDirection(AB, AO)) // outside AB plane
			{
				direction = glm::cross(glm::cross(AB, AO), AB);
				simplex.erase(simplex.begin());
			}
			else // outside A
			{
				direction = AO;
				simplex.erase(simplex.begin());
				simplex.erase(simplex.begin());
			}
		}
		else // orthogonal to face
		{
			if(isSameDirection(ABC, AO)) // outside face
			{
				direction = ABC;
			}
			else // inside face
			{
				simplex[0] = B;
				simplex[1] = C;

				direction = -ABC;
			}
		}
	}
	return false;
}

void cloth::updateFaceNormal()
{
	for(int x = 0; x<num_particles_width-1; x++)
	{
		for(int y=0; y<num_particles_height-1; y++)
		{
			vec3 normal = calcTriangleNormal(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1));
			getParticle(x+1,y)->addToNormal(normal);
			getParticle(x,y)->addToNormal(normal);
			getParticle(x,y+1)->addToNormal(normal);

			normal = calcTriangleNormal(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1));
			getParticle(x+1,y+1)->addToNormal(normal);
			getParticle(x+1,y)->addToNormal(normal);
			getParticle(x,y+1)->addToNormal(normal);
		}
	}
}

void cloth::SaveParticleVertexInfo(Particle *p1, const vec2 uv, std::vector<Vertex> &vertexData)
{
	Vertex v1 = {p1->getPos(), uv, p1->getNormal()};
	vertexData.push_back(v1);
}

void cloth::render(Shader* shader)
{
	 glPolygonMode(GL_FRONT_AND_BACK, (draw_wire ? GL_LINE : GL_FILL));
	// reset normals (which where written to last frame)
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).resetNormal();
	}

	updateFaceNormal();

	static GLuint vertexArrayObject = 0;
	static GLuint vertexBuffer = 0;
	static GLuint texture;
	static int elementSize;
	if (vertexArrayObject == 0){
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		GLuint positionAttributeLocation = glGetAttribLocation(shader->ProgramID, "position");
		GLuint uvAttributeLocation = glGetAttribLocation(shader->ProgramID, "uv");
		GLuint normalAttributeLocation = glGetAttribLocation(shader->ProgramID, "normal");
		glEnableVertexAttribArray(positionAttributeLocation);
		glEnableVertexAttribArray(uvAttributeLocation);
		glEnableVertexAttribArray(normalAttributeLocation);
		glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
		glVertexAttribPointer(uvAttributeLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec3));
		glVertexAttribPointer(normalAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(sizeof(vec3)+sizeof(vec2)));

		std::vector<int> indices;


		for (int j = 0; j < num_particles_height-1; j++) {
			int index;
			if (j > 0) {
				indices.push_back(j * num_particles_width); // make degenerate
			}
			for (int i = 0; i <= num_particles_width-1; i++) {
				index = j * num_particles_width + i;
				indices.push_back(index);
				indices.push_back(index + num_particles_width);
			}
			if (j + 1 < num_particles_height-1) {
				indices.push_back(index + num_particles_width); // make degenerate
			}
		}
		elementSize = indices.size();

		GLuint elementArrayBuffer;
		glGenBuffers(1, &elementArrayBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementSize * sizeof(int), &(indices[0]), GL_STATIC_DRAW);
		texture = loadClothTexture("clothTexture.jpg");
	}
	std::vector<Vertex> vertexData;

	for(int y=0; y<num_particles_height; y++)
	{
		for(int x = 0; x<num_particles_width; x++)
		{
			vec2 uv(x/(num_particles_width - 1.0f),y/(num_particles_height-1.0f));

			SaveParticleVertexInfo(getParticle(x, y), uv, vertexData);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), value_ptr(vertexData[0].position), GL_STREAM_DRAW);
	computeMatricesFromInputs();
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	mat4 mvp = ProjectionMatrix * ViewMatrix * ModelMatrix;
	mat4 modelView = ViewMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader->ProgramID, "mvp"),1,false, value_ptr(mvp));
	mat3 normalMatrix = inverse(transpose(mat3(modelView)));
	glUniformMatrix3fv(glGetUniformLocation(shader->ProgramID, "normalMatrix"),1,false, value_ptr(normalMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader->ProgramID, "mainTexture"), 0);

	glBindVertexArray(vertexArrayObject);
	glDrawElements(GL_TRIANGLE_STRIP, elementSize, GL_UNSIGNED_INT, 0);
}

void cloth::updatePos(float dt)
{
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).compute_predicted_position(dt); // calculate the position of each particle at the next time step.
	}
	//selfCollision();
	std::vector<Constraint>::iterator constraint;
	//printf("%i constraints size are: "+constraints.size());
	for(int i=0; i<CONSTRAINT_ITERATIONS; i++) // iterate over all constraints several times
	{
		//for(constraint = constraints.begin(); constraint != constraints.end(); c++ )
		//{
		//	(*c).applySpringForce(); // satisfy constraint.
		//}

		for(constraint = constraints.begin(); constraint != constraints.end(); constraint++ )
		{
			(*constraint).satisfyConstraint(); // satisfy constraint.
		}
	}

	for(constraint = constraints.begin(); constraint != constraints.end(); constraint++ )
	{
		//printf("fixed particles size are: %d", fixedParticles.size());
		if ((*constraint).unbearable() && fixedParticles.size()>3)
		{
			
				fixedParticles[0]->movable = true;
				fixedParticles[1]->movable = true;
				fixedParticles[2]->movable = true;
				fixedParticles.erase(fixedParticles.begin(),fixedParticles.begin()+2);
			/*fixedParticles[0]->movable = true;
			fixedParticles[1]->movable = true;
			fixedParticles[2]->movable = true;
			fixedParticles[3]->movable = true;
			fixedParticles[4]->movable = true;
			fixedParticles[5]->movable = true;
			fixedParticles.erase(fixedParticles.begin(),fixedParticles.begin()+5);*/
				
			
		}
		
	}
}

void cloth::addForce(const vec3 direction)
{
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).addForce(direction); // add the forces to each particle
	}

}

void cloth::windForce(const vec3 direction)
{
	for(int x = 0; x<num_particles_width-1; x++)
	{
		for(int y=0; y<num_particles_height-1; y++)
		{
			addWindForcesForTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1),direction);
			addWindForcesForTriangle(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1),direction);
		}
	}
}

bool cloth::ballCollision(const vec3 center,const float radius )
{
	std::vector<Particle>::iterator particle;
	collisionFlag = false;
	for(particle = particles.begin(); particle != particles.end(); particle++)
	{
		vec3 diff = (*particle).getPos()-center;//point from ball to cloth vertices 
		float dist = length(diff);
		if ( dist < radius) //intersected!
		{
			(*particle).offsetPos(normalize(diff)*(radius-dist)); // move vertices surround ball's surface
			collisionFlag = true;
		}
		
	}
	
	
	return collisionFlag;
}

bool cloth::planeCollision()
{
	float v1, v2;
	glm::vec3 planeNormal = glm::vec3(0,1,0);
	std::vector<Particle>::iterator particle;
	glm::vec3 pointOnPlane = glm::vec3(0,-16,0);
	bool flag = false;
	for(particle = particles.begin(); particle != particles.end(); particle++)
	{
		glm::vec3 pointDist = (*particle).getPos() - pointOnPlane;
		//v-f在n上的投影如果大于零表示点离平面距离大于零，未相交
		//v-f的投影如果小于零，表明点和平面相交
		float proj = dot(pointDist, planeNormal);
		float dist = abs(proj);
		//if (proj <= 0 && abs((*particle).getPos().x)<=20  && abs((*particle).getPos().z)<=20 )
		if ((*particle).getPos().y <= -16 && abs((*particle).getPos().x)<=20  && abs((*particle).getPos().z)<=20)
		{
			(*particle).pos.y = -16+0.001;
			//(*particle).offsetPos((dist+0.01f)*vec3(0,1,0));
			//printf("****************************************");
			bool flag = true;
		}
		
	}
	//if (flag)
	//{
	//	std::vector<Constraint>::iterator constraint;
	//	for(int i=0; i<5; i++) // iterate over all constraints several times
	//	{
	//		for(constraint = constraints.begin(); constraint != constraints.end(); constraint++ )
	//		{
	//			(*constraint).satisfyConstraint(); // satisfy constraint.
	//		}
	//	}
	//}
	
	return flag;
}


GLuint cloth::loadClothTexture(const char* path)
{
	//Generate texture ID and load texture data 

	glGenTextures(1, &clothTextureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, clothTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return clothTextureID;
}

//bool cloth::NarrowPhaseCheck(Face faceA, Face faceB)
//{
//	// Support function
//	//first direction(mess distance of two obj)
//	vec3 direction = faceA.particleA->getPos() - faceB.particleA->getPos();
//
//
//	// Minkowski difference
//	vec3 MinkowskiDiff = supportFunction(objI, objJ, direction);
//
//	// Simplex points
//	std::vector<vec3> simplex;
//	//collection of simplex points 
//	//before the loop begin, there is only 1 point-old point
//	simplex.push_back(MinkowskiDiff);
//
//	// second direction, move towards origin
//	direction = -MinkowskiDiff;
//
//	int count = 1000;
//	while(count > 0)
//	{
//		count--;
//		MinkowskiDiff = supportFunction(objI, objJ, direction);
//
//		//decide if Minkowski difference contains origin
//		if(dot(MinkowskiDiff, direction) < 0)
//		{
//			return NULL;//no intersection
//		}
//
//		//intersection, push new point to simplex
//		simplex.push_back(MinkowskiDiff);
//
//		//update new simplex and new direction to origin
//		if(DoSimplex(simplex, direction))
//		{
//			objI->narrowCollision = true;
//			objJ->narrowCollision = true;
//			//now the simplex is the result of GJK,find the vec3 from origin to the nearest on the MK
//			vec3 normal = findContactNormal(objI, objJ, simplex);
//			if(normal != vec3(vec3::null))
//			{	
//				std::vector<vec3> contactPoints = findContactPoints(objI, objJ, normal);
//				if(contactPoints.size() != 2)
//					return new contactModel(normal, objI->messPos, objJ->messPos);
//
//				return new contactModel(normal, contactPoints[0], contactPoints[1]);
//			}
//
//			return NULL;
//		}else{
//			objI->narrowCollision = false;
//			objJ->narrowCollision = false;
//		}
//	}
//
//	//return NULL;
//}
//
//vec3 cloth::supportFunction(Face faceA, Face faceB, vec3 &direction) 
//{
//	vec3 furthestI = objI->getFurthestPointInDirection(direction);
//	vec3 furthestJ = objJ->getFurthestPointInDirection(-direction);
//
//	// Minkowski difference
//	vec3 p = furthestI - furthestJ;
//
//	return  p;
//}
//
//
//
////http://in2gpu.com/2014/05/18/gjk-algorithm-3d/
//bool CollisionDetector::DoSimplex(std::vector<vec3> &simplex, vec3 &direction)
//{
//	vec3 A, B, C, D;
//	//simplex type-line? triangle? tetrahedron?
//	int pointCount = simplex.size();
//	if (pointCount == 1)
//	{
//		direction = -simplex[0];
//		return false;
//	}
//	if (pointCount == 2)
//	{
//		return DoLineSimplex(simplex, direction);
//	}
//	if (pointCount == 3)
//	{
//		return DoTriangleSimplex(simplex, direction);
//	}
//	if (pointCount == 4)
//	{
//		return DoTetrahedronSimplex(simplex, direction);
//	}
//}

bool cloth::selfCollisionResponse(Face face, Particle* m_q)
{
	glm::vec3 q, p1, p2, p3;
	q =  m_q->getPos();
	p1 = face.particleA->getPos();
	p2 =  face.particleB->getPos();
	p3 =  face.particleC->getPos();

	q = q - p1;
	p2 = p2 - p1;
	p3 = p3 - p1;
	p1 = glm::vec3(0.0f);

	glm::vec3 normal(glm::cross(p2, p3));
	float c23 = glm::length(normal);
	normal = glm::normalize(normal);

	float value = glm::dot(q, normal) - clothThick;
	if(value > 0.0f)
		return true;

	glm::vec3 dcq, dcp1, dcp2, dcp3;
	dcq = normal;
	dcp2 = (glm::cross(p3, q) + glm::cross(normal, p3) * glm::dot(normal, q)) / c23;
	dcp3 = -(glm::cross(p2, q) + glm::cross(normal, p2) * glm::dot(normal, q)) / c23;
	dcp1 = -dcq - dcp2 - dcp3;

	float wq, w1, w2, w3;
	wq = 1;
	w1 = face.particleA->mass;
	w2 = face.particleB->mass;;
	w3 = face.particleC->mass;;

	float denominator = w1 * glm::dot(dcp1, dcp1) + w2 * glm::dot(dcp2, dcp2) + w3 * glm::dot(dcp3, dcp3) + wq * glm::dot(dcq, dcq);
	assert(denominator < 0.0001);

	glm::vec3 dq, dp1, dp2, dp3;
	float s = value / denominator;
	dq = -wq * s * dcq;
	dp1 = -w1 * s * dcp1;
	dp2 = -w2 * s * dcp2;
	dp3 = -w3 * s * dcp3;

	m_q->pos += dq;//set stiffness equals to 1
	face.particleA->pos += dp1;
	face.particleB->pos += dp2;
	face.particleC->pos += dp3;

	return false;
}


// Determine whether a ray intersect with a triangle
// Parameters
// orig: origin of the ray
// dir: direction of the ray
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection

