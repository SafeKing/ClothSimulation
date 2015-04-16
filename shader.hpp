#pragma once

#include <GL/glew.h>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string> 
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>


class Shader
{
public:
	Shader();
	~Shader();

	GLuint vPositionID, vColorID, vNormalID;
	GLuint vUvID, fTextureID;
	GLuint vTangentID;
	GLuint viewID, projectionID, modelID, MatrixID;

	GLuint DiffuseTextureID;
	GLuint NormalTextureID;
	GLuint SpecularTextureID;
	glm::vec4 lightPos0;
	glm::vec4 lightPos1;
	GLuint cameraPosID;

	GLuint fNormalTextureID;
	GLuint fCubeTextureID;

	GLuint ambientColorID, ambientIntensityID;	
	GLuint diffuseColorID, diffuseIntensityID, diffuseDirectionID,LightID;
	GLuint specularColorID, specularIntensityID, specularShininessID;
	GLuint orenNayarRoughnessID;
	GLuint ReflectFactorID;
	GLuint translucentEtaID;

	GLuint ModelView3x3MatrixID;

	glm::mat4 mV, mP, mM;

	GLuint ProgramID;

	GLuint GetProgramID();

	bool LoadFile(const std::string& fileName, std::string& outShader);
	char* loadComputeFile(const char *name);

	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);

	GLuint CompileShaders(const std::string& vsFilename, const std::string& psFilename);

	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

	void initShaders();

	GLuint ComputeByGPU(const char* computeShaderName);

	void checkCompileStatus(GLuint shader, const char *shadername);
	void checkLinkStatus(GLuint program, const char * programName);

	void sendMatrixToShader(GLuint matrixID, glm::mat4 &matrix);
	void UseProgram(GLuint ProgramID);
	void UseTexture(GLuint textureID);
	void UseCubeMapTexture(GLuint textureID);

	void DisableTexture();
	void DeleteCubeMapTexture();


	void SetCameraPos(glm::vec3 &cameraPos);
	void SetAmbientComponent(glm::vec3 color, float intensity);
	void SetLightDirection(glm::vec3 direction, glm::vec3 color, float intensity);
	void SetSpecularComponent(glm::vec3 color, float intensity, float shininess);
	void SetTransmitEta(glm::vec3 &eta);
	void SetReflectFactor(float reflectFactor);
	
};


