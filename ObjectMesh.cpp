#include "ObjectMesh.h"


ObjectMesh::ObjectMesh(const char * path)
{
	//loadAssImp( path);

}

ObjectMesh::ObjectMesh(void)
{
}


ObjectMesh::~ObjectMesh(void)
{
}

//bool ObjectMesh::loadAssImp(
//	const char * path
//	/*std::vector<unsigned short> & indices,
//	std::vector<glm::vec3> & vertices,
//	std::vector<glm::vec2> & uvs,
//	std::vector<glm::vec3> & normals*/
//	){
//		indices.clear();
//		vertices.clear();
//		uvs.clear();
//		normals.clear();
//
//		Assimp::Importer importer;
//
//		const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
//		if( !scene) {
//			fprintf( stderr, importer.GetErrorString());
//			getchar();
//			return false;
//		}	
//		printf("  %i animations\n", scene->mNumAnimations);
//		printf("  %i cameras\n", scene->mNumCameras);
//		printf("  %i lights\n", scene->mNumLights);
//		printf("  %i materials\n", scene->mNumMaterials);
//		printf("  %i meshes\n", scene->mNumMeshes);
//		printf("  %i textures\n", scene->mNumTextures);
//
//		// Fill vertices positions
//		const aiMesh* mesh = scene->mMeshes[0]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)
//
//		verticesNumber = mesh->mNumVertices;
//		vertices.reserve(mesh->mNumVertices);
//		for(unsigned int i=0; i<verticesNumber; i++){
//			aiVector3D pos = mesh->mVertices[i];
//			vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
//		}
//		printf("vertices loaded!\n");
//
//		// Fill vertices texture coordinates
//		uvs.reserve(mesh->mNumVertices);
//		for(unsigned int i=0; i<mesh->mNumVertices; i++){
//			aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
//			uvs.push_back(glm::vec2(UVW.x, UVW.y));
//		}
//		printf("uv loaded!\n");
//
//		// Fill vertices normals
//		normals.reserve(mesh->mNumVertices);
//		for(unsigned int i=0; i<mesh->mNumVertices; i++){
//			aiVector3D n = mesh->mNormals[i];
//			normals.push_back(glm::vec3(n.x, n.y, n.z));
//		}
//		printf("normals loaded!\n");
//
//		// Fill face indices
//		indices.reserve(3*mesh->mNumFaces);
//		for (unsigned int i=0; i<mesh->mNumFaces; i++){
//			// Assume the model has only triangles.
//			indices.push_back(mesh->mFaces[i].mIndices[0]);
//			indices.push_back(mesh->mFaces[i].mIndices[1]);
//			indices.push_back(mesh->mFaces[i].mIndices[2]);
//		}
//		printf("indices loaded!\n");
//		// The "scene" pointer will be deleted automatically by "importer"
//		printf("mesh loaded!\n");
//
//		importer.FreeScene();
//
//		return true;
//}

void ObjectMesh::GenerateObjectBuffer()
{
	////Initialize VAO
	//glGenVertexArrays( 1, &ObjVAO );
	//glBindVertexArray( ObjVAO );

	//Calc Array Sizes
	GLuint vertSize = vertices.size() * sizeof(glm::vec3);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertSize, &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);
	

}

void ObjectMesh::LinkObjToShader(Shader* shader)
{	

	//Initialize VAO
	glGenVertexArrays( 1, &ObjVAO );
	glBindVertexArray( ObjVAO );

	//// Bind our texture in Texture Unit 0
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, Texture);
	//glUniform1i(TextureID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);


	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ObjectMesh::Render(Shader * shader)
{


	glBindVertexArray (ObjVAO);
	glDrawArrays (GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}

void ObjectMesh::drawSphere(glm::vec3& ballPos, Shader* shader){
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(shader->ProgramID);
	sphereVAO = 0;

	if (sphereVAO == 0){
		
		std::vector<Vertex> sphereData;
		int slices = 64;
		int stacks = 32;
		ball_radius = 1.4;
		int vertexCount = (stacks+1) * (slices + 1);
		float piDivStacks = M_PI / stacks;
		float PIDiv2 = M_PI / 2;
		float PI2 = M_PI * 2;

		for (int j = 0; j <= stacks; j++) {
			float latitude1 = piDivStacks * j - PIDiv2;
			float sinLat1 = sin(latitude1);
			float cosLat1 = cos(latitude1);
			for (int i = 0; i <= slices; i++) {
				float longitude = (PI2 / slices) * i;
				float sinLong = sin(longitude);
				float cosLong = cos(longitude);
				glm::vec3 normal = glm::vec3(cosLong * cosLat1, sinLat1, sinLong * cosLat1);
				glm::vec3 position = normal * ball_radius;
				Vertex v = {position, glm::vec2(j/(float)stacks, i/(float)slices), normal};
				sphereData.push_back(v);
			}
		}
		std::vector<GLuint> indices;
		// create indices
		for (int j = 0; j < stacks; j++) {
			int index;
			if (j > 0) {
				indices.push_back(j * (slices + 1)); // make degenerate
			}
			for (int i = 0; i <= slices; i++) {
				index = j * (slices + 1) + i;
				indices.push_back(index);
				indices.push_back(index + slices + 1);
			}
			if (j + 1 < stacks) {
				indices.push_back(index + slices + 1); // make degenerate
			}
		}

		glGenVertexArrays(1, &sphereVAO);
		glBindVertexArray(sphereVAO);

		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sphereData.size() * sizeof(Vertex), glm::value_ptr(sphereData[0].position), GL_STATIC_DRAW);

		GLuint positionAttributeLocation = glGetAttribLocation(shader->ProgramID, "position");
		GLuint uvAttributeLocation = glGetAttribLocation(shader->ProgramID, "uv");
		GLuint normalAttributeLocation = glGetAttribLocation(shader->ProgramID, "normal");
		glEnableVertexAttribArray(positionAttributeLocation);
		glEnableVertexAttribArray(uvAttributeLocation);
		glEnableVertexAttribArray(normalAttributeLocation);
		glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
		glVertexAttribPointer(uvAttributeLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(glm::vec3));
		glVertexAttribPointer(normalAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(sizeof(glm::vec3)+sizeof(glm::vec2)));

		GLuint elementArrayBuffer;
		glGenBuffers(1, &elementArrayBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &(indices[0]), GL_STATIC_DRAW);
		elementCount = indices.size();
		glm::vec4 color = glm::vec4(0.4, 0.8, 0.5, 1.0);
		//sphereTex = buildCandyColorTexture(color, color, 1);
		sphereTex = loadObjectTexture("qinghua.jpg");
	}

	computeMatricesFromInputs();
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	ModelMatrix = glm::translate(ModelMatrix, ballPos);
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 mvp = ProjectionMatrix * ViewMatrix * ModelMatrix;
	glm::mat4 modelView = ViewMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader->ProgramID, "mvp"),1,false, glm::value_ptr(mvp));
	glm::mat3 normalMatrix = glm::inverse(glm::transpose(glm::mat3(modelView)));
	glUniformMatrix3fv(glGetUniformLocation(shader->ProgramID, "normalMatrix"),1,false, glm::value_ptr(normalMatrix));

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, sphereTex);
	glUniform1i(glGetUniformLocation(shader->ProgramID, "mainTexture"),0);

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, elementCount, GL_UNSIGNED_INT, 0);
}

void ObjectMesh::CreateMesh()
{
	vertices.clear();
	uvs.clear();

	vertices.push_back(glm::vec3(-1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(-1.0f,-1.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f,-1.0f));
	vertices.push_back(glm::vec3(-1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f,-1.0f));
	vertices.push_back(glm::vec3(1.0f,-1.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f,-1.0f));
	vertices.push_back(glm::vec3(1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(-1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(-1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f,-1.0f));
	vertices.push_back(glm::vec3(1.0f,-1.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f,-1.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f,-1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f,-1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f,-1.0f));
	vertices.push_back(glm::vec3(1.0f,-1.0f,-1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f,-1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f,-1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f,-1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f,-1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f,-1.0f, 1.0f));



	uvs.push_back(glm::vec2(0.000059f, 0.000004f));
	uvs.push_back(glm::vec2(0.000103f, 0.336048f));
	uvs.push_back(glm::vec2(0.335973f, 0.335903f));
	uvs.push_back(glm::vec2(1.000023f, 0.000013f));
	uvs.push_back(glm::vec2(0.667979f, 0.335851f));
	uvs.push_back(glm::vec2(0.999958f, 0.336064f));
	uvs.push_back(glm::vec2(0.667979f, 0.335851f));
	uvs.push_back(glm::vec2(0.336024f, 0.671877f));
	uvs.push_back(glm::vec2(0.667969f, 0.671889f));
	uvs.push_back(glm::vec2(1.000023f, 0.000013f));
	uvs.push_back(glm::vec2(0.668104f, 0.000013f));
	uvs.push_back(glm::vec2(0.667979f, 0.335851f));
	uvs.push_back(glm::vec2(0.000059f, 0.000004f));
	uvs.push_back(glm::vec2(0.335973f, 0.335903f));
	uvs.push_back(glm::vec2(0.336098f, 0.000071f));
	uvs.push_back(glm::vec2(0.667979f, 0.335851f));
	uvs.push_back(glm::vec2(0.335973f, 0.335903f));
	uvs.push_back(glm::vec2(0.336024f, 0.671877f));
	uvs.push_back(glm::vec2(1.000004f, 0.671847f));
	uvs.push_back(glm::vec2(0.999958f, 0.336064f));
	uvs.push_back(glm::vec2(0.667979f, 0.335851f));
	uvs.push_back(glm::vec2(0.668104f, 0.000013f));
	uvs.push_back(glm::vec2(0.335973f, 0.335903f));
	uvs.push_back(glm::vec2(0.667979f, 0.335851f));
	uvs.push_back(glm::vec2(0.335973f, 0.335903f));
	uvs.push_back(glm::vec2(0.668104f, 0.000013f));
	uvs.push_back(glm::vec2(0.336098f, 0.000071f));
	uvs.push_back(glm::vec2(0.000103f, 0.336048f));
	uvs.push_back(glm::vec2(0.000004f, 0.671870f));
	uvs.push_back(glm::vec2(0.336024f, 0.671877f));
	uvs.push_back(glm::vec2(0.000103f, 0.336048f));
	uvs.push_back(glm::vec2(0.336024f, 0.671877f));
	uvs.push_back(glm::vec2(0.335973f, 0.335903f));
	uvs.push_back(glm::vec2(0.667969f, 0.671889f));
	uvs.push_back(glm::vec2(1.000004f, 0.671847f));
	uvs.push_back(glm::vec2(0.667979f, 0.335851f));
}

void ObjectMesh::generatePlaneBuffer(Shader* shader)
{
	glUseProgram(shader->ProgramID);
	for(unsigned int i = 0; i < vertices.size(); i += 3)
	{
		glm::vec3 v1 = vertices[i+1] - vertices[i];
		glm::vec3 v2 = vertices[i+2] - vertices[i];

		glm::vec3 norm = glm::normalize(glm::cross(v1, v2));
		normals.push_back(norm);
		normals.push_back(norm);
		normals.push_back(norm);
	}

	//Initialize VAO
	glGenVertexArrays( 1, &PlaneVAO );
	glBindVertexArray( PlaneVAO );

	//Calc Array Sizes
	GLuint vertexArraySize = vertices.size()*sizeof(glm::vec3);

	//Initialize VBO
	glGenBuffers( 1, &vertexbuffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
	glBufferData( GL_ARRAY_BUFFER, vertexArraySize,  &vertices[0], GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	 glm::vec4 color = glm::vec4(0.5, 0.5, 0.3, 1.0);
	TexturePlane = buildCandyColorTexture(color, color, 1);
	
	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void ObjectMesh::drawPlane(Shader* shader)
{
	computeMatricesFromInputs();
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(20,1,20));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0,-17,0));
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 mvp = ProjectionMatrix * ViewMatrix * ModelMatrix;
	glm::mat4 modelView = ViewMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader->ProgramID, "mvp"),1,false, glm::value_ptr(mvp));
	glm::mat3 normalMatrix = glm::inverse(glm::transpose(glm::mat3(modelView)));
	glUniformMatrix3fv(glGetUniformLocation(shader->ProgramID, "normalMatrix"),1,false, glm::value_ptr(normalMatrix));

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, TexturePlane);
	glUniform1i(glGetUniformLocation(shader->ProgramID, "mainTexture"),0);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(PlaneVAO);
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	glBindVertexArray(0);

}
