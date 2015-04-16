#include "shader.hpp"


Shader::Shader()
{
}


Shader::~Shader()
{
	glDeleteProgram(ProgramID);
}

bool Shader::LoadFile(const std::string& fileName, std::string& outShader)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		std::cout << "Error Loading file: " << fileName << " - impossible to open file" << std::endl;
		return false;
	}

	if (file.fail())
	{
		std::cout << "Error Loading file: " << fileName << std::endl;
		return false;
	}

	std::stringstream stream;
	stream << file.rdbuf();
	file.close();

	outShader = stream.str();

	return true;
}

char* Shader::loadComputeFile(const char *name){
	FILE *filePointer;
	char *content = NULL;

	int count = 0;

	if (name != NULL) {
		filePointer = fopen(name, "rt");

		if (filePointer != NULL) {

			fseek(filePointer, 0, SEEK_END);
			count = ftell(filePointer);
			rewind(filePointer);

			if (count > 0) {
				content = (char *)malloc(sizeof(char)* (count + 1));
				count = fread(content, sizeof(char), count, filePointer);
				content[count] = '\0';
			}
			fclose(filePointer);
		}
	}
	return content;
}

void Shader::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}
	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderText, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint Shader::CompileShaders(const std::string& vsFilename, const std::string& psFilename)
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	GLuint shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	std::string vs, ps;
	LoadFile(vsFilename, vs);
	AddShader(shaderProgramID, vs.c_str(), GL_VERTEX_SHADER);
	LoadFile(psFilename, ps);
	AddShader(shaderProgramID, ps.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}

GLuint Shader::LoadShaders(const char * vertex_file_path, const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()){
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}



	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint Shader::ComputeByGPU(const char* computeShaderName)
{
	const char *computeShaderSource = loadComputeFile(computeShaderName);
	GLuint computeShaderID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShaderID, 1, &computeShaderSource, NULL);
	glCompileShader(computeShaderID);
	checkCompileStatus(computeShaderID, computeShaderName);

	GLuint program = glCreateProgram();
	glAttachShader(program, computeShaderID);
	glLinkProgram(program);
	checkLinkStatus(program, computeShaderName);
	return program;
}

void Shader::checkCompileStatus(GLuint shader, const char *shadername)
{
	GLint  compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cerr << shadername << " failed to compile:" << std::endl;
		GLint  logSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetShaderInfoLog(shader, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;
	}
}

void Shader::checkLinkStatus(GLuint program, const char * programName)
{
	GLint  linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		std::cerr << "Shader program " << programName << " failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;
		system("Pause");
		exit(0);
	}
}


GLuint Shader::GetProgramID()
{
	return ProgramID;
}

void Shader::initShaders()
{

	 lightPos0 = glm::vec4(-1.0,1.0,0.5,0.0);
	glm::vec4 lightAmbient0 = glm::vec4(0.2, 0.2, 0.2, 1.0);
	glm::vec4 lightDiffuse0 = glm::vec4(0.8, 0.8, 0.8, 1.0);

	lightPos1 = glm::vec4(1.0,0.0,-0.2,0.0);
	glm::vec4 lightAmbient1 = glm::vec4(0.0,0.0,0.0,0.0);
	glm::vec4 lightDiffuse1 = glm::vec4(0.5,0.5,0.3,0.0);

	glm::vec4 ambient[2] = {lightAmbient0, lightAmbient1};
	glm::vec4 diffuse[2] = {lightDiffuse0, lightDiffuse1};
	glUseProgram(ProgramID);
	glUniform4fv(glGetUniformLocation(ProgramID, "lightAmbient"),2, glm::value_ptr(ambient[0]));
	glUniform4fv(glGetUniformLocation(ProgramID, "lightDiffuse"),2, glm::value_ptr(diffuse[0]));

	glm::vec4 lightModelAmbient = glm::vec4(0.2, 0.2, 0.2, 1.0);
	glUniform4fv(glGetUniformLocation(ProgramID, "lightModelAmbient"),1, glm::value_ptr(lightModelAmbient));
	glm::vec4 eyeSpaceLight[2] = { lightPos0, lightPos1};
	glUniform4fv(glGetUniformLocation(ProgramID, "lightPosition"), 2, glm::value_ptr(eyeSpaceLight[0]));
}


void Shader::sendMatrixToShader(GLuint matrixID, glm::mat4 &matrix)
{
	glUseProgram(ProgramID);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE,  &matrix[0][0]);
}
void Shader::UseProgram(GLuint ProgramID)
{
	glUseProgram(ProgramID);
}


void Shader::UseTexture(GLuint textureID)
{
	glEnable(GL_TEXTURE_2D);

	glUniform1i(fTextureID, 0);
	glActiveTexture(GL_TEXTURE0);
	
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Shader::DisableTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
}

void Shader::UseCubeMapTexture(GLuint textureID)
{
	glEnable(GL_TEXTURE_CUBE_MAP);

	glUniform1i(fCubeTextureID, 0);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void Shader::DeleteCubeMapTexture()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDisable(GL_TEXTURE_CUBE_MAP);
}

void Shader::SetCameraPos(glm::vec3 &cameraPos)
{
	glUseProgram(ProgramID);
	glUniform3f(cameraPosID, cameraPos.x, cameraPos.y, cameraPos.z);
}

void Shader::SetAmbientComponent(glm::vec3 color, float intensity)
{
	glUseProgram(ProgramID);
	glUniform3f(ambientColorID, color.r, color.g, color.b);
	glUniform1f(ambientIntensityID, intensity);
}

void Shader::SetLightDirection(glm::vec3 direction, glm::vec3 color, float intensity)
{
	glUseProgram(ProgramID);
	glUniform3f(diffuseColorID, color.r, color.g, color.b);
	glUniform1f(diffuseIntensityID, intensity);
	glUniform3f(diffuseDirectionID, direction.x, direction.y, direction.z);
}

void Shader::SetSpecularComponent(glm::vec3 color, float intensity, float shininess)
{
	glUseProgram(ProgramID);
	glUniform3f(specularColorID, color.r, color.g, color.b);
	glUniform1f(specularIntensityID, intensity);
	glUniform1f(specularShininessID, shininess);
}

void Shader::SetTransmitEta(glm::vec3 &eta)
{
	glUseProgram(ProgramID);
	glUniform3f(translucentEtaID, eta.r, eta.g, eta.b);
}

void Shader::SetReflectFactor(float reflectFactor)
{
	glUseProgram(ProgramID);
	glUniform1f(ReflectFactorID, reflectFactor);
}