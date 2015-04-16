#pragma once

#include <vector>
#include <GL/glew.h>
// Include GLM

#include <glm/gtc/matrix_transform.hpp>
// Include AssImp
//#include <assimp/Importer.hpp>      // C++ importer interface
//#include <assimp/scene.h>           // Output data structure
//#include <assimp/postprocess.h>     // Post processing flags
#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"

#define M_PI       3.14159265358979323846


#define BUFFER_OFFSET(i) ((char *)NULL + (i))
class ObjectMesh
{
public:
	ObjectMesh(const char * path);
	ObjectMesh(void);
	~ObjectMesh(void);

	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	float ball_radius;
	glm::vec3 ball_pos;
	GLuint ObjVAO, vertexbuffer,uvbuffer,normalbuffer,elementbuffer,tangentbuffer,bitangentbuffer;
	GLuint objTextureID;
	GLuint BumpObjVAO;
	GLuint PlaneVAO;
	int verticesNumber;

	GLuint DiffuseTexture ;
	GLuint NormalTexture;
	GLuint SpecularTexture ;

	 GLuint sphereVAO;
	 int elementCount;
	 GLuint sphereTex, TexturePlane;

	bool loadAssImp(
		const char * path
		/*std::vector<unsigned short> & indices,
		std::vector<glm::vec3> & vertices,
		std::vector<glm::vec2> & uvs,
		std::vector<glm::vec3> & normals*/
		);
	
	void GenerateObjectBuffer();

	void LinkObjToShader(Shader* shader);
	void Render(Shader * shader);

	void drawSphere(glm::vec3& position, Shader* shader);
	void CreateMesh();
	void generatePlaneBuffer(Shader* shader);
	void drawPlane(Shader* shader);
};

