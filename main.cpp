//////////////////////////////////////////////////////////////////////////////////
//////THIS CODE IS BUILD BASED ON Mosegaards Cloth Simulation Coding Tutorial/////
//////http://cg.alexandra.dk/?p=147                                          /////
//////////////////////////////////////////////////////////////////////////////////
// Include standard headers
#include <stdio.h>

#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>
//#include <glm/gtx/euler_angles.hpp>
//#include <glm/gtx/norm.hpp>
#include "cloth.h"
#include "Particle.h"
#include "Shader.hpp"
#include "texture.hpp"
#include "controls.hpp"

#include "ObjectMesh.h"
#include <AntTweakBar.h>
#define MESH "teapot2.obj"

#define NUM_PARTICLES_WIDTH 30
#define NUM_PARTICLES_HEIGHT 50
#define CLOTH_WIDTH 9.0f
#define CLOTH_HEIGHT 16.0f
#define DAMPING 0.01f // how much to damp the cloth simulation each frame
#define TIME_STEPSIZE_SQUARE 0.5f*0.5f*0.5f// how large time step each particle takes each frame
#define CONSTRAINT_ITERATIONS 15 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)
using namespace glm;

Shader* litShader;
ObjectMesh* ball;
ObjectMesh* plane;
cloth* cloth1;
//vec3 ball_pos; // the center of our one ball
//float ball_radius;
Shader* reflectShader;
TwBar * quaternionGUI;
bool drawCool;
float stiffness;
int iterative;
//typedef void (* GLFWmousebuttonfun)(GLFWwindow*,int,int,int);
void MouseButtonCB( GLFWwindow*,int button ,int action ,int mods)
{
	TwEventMouseButtonGLFW( button , action );
}

//typedef void (* GLFWcursorposfun)(GLFWwindow*,double,double);
void MousePosCB(GLFWwindow*,double x ,double y)
{
	TwEventMousePosGLFW( (int)x, (int)y );
}

//typedef void (* GLFWkeyfun)(GLFWwindow*,int,int,int,int);
void KeyFunCB( GLFWwindow* window,int key,int scancode,int action,int mods)
{
	TwEventKeyGLFW( key , action );
	TwEventCharGLFW( key  , action );
}

//typedef void (* GLFWscrollfun)(GLFWwindow*,double,double);
void MouseScrollCB(  GLFWwindow* window, double x , double y )
{
	TwEventMouseWheelGLFW( (int)y );
}

void initGUI(){
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(1024, 768);
	
	quaternionGUI = TwNewBar("Reflection setting ");
	
//	TwSetParam(QuaternionGUI, NULL, "position", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(quaternionGUI, "stiffness", TW_TYPE_FLOAT, &stiffness, "step=0.01" " label='Spring stiffness '" );
	TwAddVarRW(quaternionGUI, "IterativeTimes", TW_TYPE_FLOAT, &iterative, "step=1" " label='Iterative times'" );





}

int initWindow(){
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Cloth simulation", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS); 
	glDepthFunc(GL_LEQUAL);
	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	/* Set GLFW event callbacks */
	// - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetMouseButtonCallback( window , MouseButtonCB );
	// - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetCursorPosCallback( window , MousePosCB);
	// - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetScrollCallback( window , MouseScrollCB );
	// - Directly redirect GLFW key events to AntTweakBar
	glfwSetKeyCallback(window , KeyFunCB);
}
void init()
{
	litShader = new Shader();
	litShader-> ProgramID = litShader->LoadShaders( "phong.vert", "phong.frag" );
	litShader->initShaders();

	ball = new ObjectMesh();
	ball->ball_pos = vec3(7,-9,5);
	ball->ball_radius = 1.4; // the radius of our one ball

	plane = new ObjectMesh();
	plane->CreateMesh();
	plane->generatePlaneBuffer(litShader);
	//cloth1 = new cloth(14,10,55,45); // one Cloth object of the Cloth class
	cloth1 = new cloth(CLOTH_WIDTH,CLOTH_HEIGHT,NUM_PARTICLES_WIDTH,NUM_PARTICLES_HEIGHT);
	stiffness = 1;
	
	for(int i=0;i<3; i++)
	{
		/*cloth1->setFixedPoints(0+i ,0);
		cloth1->setFixedPoints(NUM_PARTICLES_WIDTH-1-i ,0);
		cloth1->setFixedPoints(0+i ,NUM_PARTICLES_HEIGHT-1);*/
		cloth1->setFixedPoints(NUM_PARTICLES_WIDTH-1-i ,NUM_PARTICLES_HEIGHT-1);
	}
	for(int i=0;i<3; i++)
	{
		cloth1->setFixedPoints(0+i ,NUM_PARTICLES_HEIGHT-1);
	}
	for(int i=0;i<3; i++)
	{
		cloth1->setFixedPoints(NUM_PARTICLES_WIDTH-1-i ,0);
	}
	for(int i=0;i<3; i++)
	{
		cloth1->setFixedPoints(0+i ,0);
	}

	
}


void keyEventHandle(){
	bool flag = false;
	bool drawflag = false;
	if (glfwGetKey(window, GLFW_KEY_R ) == GLFW_PRESS)
	{
		drawflag = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R ) == GLFW_RELEASE && drawflag)
	{
		cloth1->draw_wire = !cloth1->draw_wire;
	}
	if (glfwGetKey(window, GLFW_KEY_E ) == GLFW_PRESS)
	{
		flag = true;
	}
	if (glfwGetKey(window, GLFW_KEY_E ) == GLFW_RELEASE && flag)
	{
		drawCool=!drawCool;
	}
	if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS)
	{
		ball->ball_pos.x -= 0.1;
	}
	if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS)
	{
		ball->ball_pos.x += 0.1;
	}
	if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS)
	{
		ball->ball_pos.y += 0.1;
	}
	if (glfwGetKey(window, GLFW_KEY_X ) == GLFW_PRESS)
	{
		ball->ball_pos.y -= 0.1;
	}
	if (glfwGetKey(window, GLFW_KEY_Z ) == GLFW_PRESS)
	{
		ball->ball_pos.z += 0.1;
	}
	if (glfwGetKey(window, GLFW_KEY_C ) == GLFW_PRESS)
	{
		ball->ball_pos.z -= 0.1;
	}
	if (glfwGetKey(window, GLFW_KEY_U ) == GLFW_PRESS)
	{
		for (int i =0; i<cloth1->fixedParticles.size();i++)
		{
			cloth1->fixedParticles[i]->movable = true;
		}
		cloth1->fixedParticles.clear();
	}
}
int main( void )
{

	initWindow();
	initGUI();

	init();
	printf("ball position is %f, %f, %f\n",ball->ball_pos.x, ball->ball_pos.y, ball->ball_pos.z);
	float ball_time = 0; 
	do{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		keyEventHandle();
		cloth1->iterativeTimes = iterative;
		ball_time++;
	
		//ball->ball_pos.z = (float)cos(ball_time/50.0f)*7;

		std::vector<Constraint>::iterator constraint;
		for(constraint = cloth1->constraints.begin(); constraint != cloth1->constraints.end(); constraint++ )
		{
			(*constraint).set_stiffness(stiffness); // satisfy constraint.
		}
		float iSecret = -0.1f *(float)(rand() % 4);
		cloth1->addForce(vec3(0,-0.2,0)*TIME_STEPSIZE_SQUARE); // add gravity each frame, pointing down
		cloth1->windForce((vec3(0.2,0,0.2)+vec3(iSecret,0,iSecret))*TIME_STEPSIZE_SQUARE); // generate some wind each frame
		cloth1->updatePos(TIME_STEPSIZE_SQUARE); // calculate the particle positions of the next frame
	
		cloth1->ballCollision(ball->ball_pos,ball->ball_radius + cloth1->clothThick);
		cloth1->planeCollision();
		cloth1->selfCollision();
		//cloth1->collisionDetection(ball->ball_pos,ball->ball_radius);

		glEnable(GL_LIGHTING);
		glUseProgram(litShader->ProgramID);
		GLuint dsbs = glGetUniformLocation(litShader->ProgramID, "cool");
		glUniform1i(dsbs, drawCool);
		cloth1->render(litShader);
	
		ball->drawSphere(ball->ball_pos, litShader);
		//drawball(ball->ball_pos);

		plane->drawPlane(litShader);
		TwDraw();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

