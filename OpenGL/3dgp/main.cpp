#include <iostream>
#include <variant>
#include <vector>
#include <string>

#ifndef GLM_FORCE_INLINE
#define GLM_FORCE_INLINE
#endif
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <3dgl/3dgl.h>

#include "Globals.h"
#include "Program.h"
#include "Transform.h"
#include "PostEffects.h"
#include "Light.h"
#include "Properties.h"
#include "Shadow.h"
#include "Textures.h"
#include "Models.h"
#include "Cube.h"
#include "MyAnimation.h"

#pragma comment (lib, "glew32.lib")


//Testing
glm::vec3 test = glm::vec3(-0.1, 0.9, 0.1);
//Temp
int Post0_bools = 0; //0xf;
bool mouse_first_grab = true;
bool blur = false;


//Shadows
Shadow shadows[2];

using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
using glm::normalize;
using glm::ortho;
using glm::rotate;
using glm::radians;
using Globals::win_h;
using Globals::win_w;
using Globals::dematerializer;

namespace {
	auto& Posts = PostEffects::Posts;
	auto& idFBO = PostEffects::idFBO;
	auto& idDepthBuffer = PostEffects::idDepthBuffer;
	auto& idVBO = PostEffects::idVBO;
	
	auto& programs = Program::programs;

	auto& lights = Light::MyLights;
	auto GetAmbient = [](auto&&... args) { return Light::GetAmbient(std::forward<decltype(args)>(args)...); };
	auto SetUpLight = [](auto&&... args) { return Light::SetUpLight(std::forward<decltype(args)>(args)...); };

	auto& ShadowPassing = Shadow::ShadowPassing;

	auto& cnt_gl_texture_id_count = Textures::cnt_gl_texture_id_count;
	auto& idTex = Textures::idTex;
	auto& textures = Textures::textures;

	auto AddNewPost = [](auto&&... args) { return PostEffects::AddNewPost(std::forward<decltype(args)>(args)...); };
	auto& AddNew2DTexture = Textures::AddNew2DTexture;
	auto& ApplyPosts = PostEffects::ApplyPosts;

	auto SetUpModel = [](auto&&... args) { return Models::SetUpModel(std::forward<decltype(args)>(args)...); };
	auto& SetUpModels = Models::SetUpModels;
	auto& SetUpMatrix = Models::SetUpMatrix;
	auto& ModelsInit = Models::ModelsInit;
	//auto& SendProgramSpecificUniforms = Models::SendProgramSpecificUniforms;
	auto& models = Models::models;
}

float lastTime = 0;
unsigned char bools = 3; //NULL | NULL | lamp1 closing | lamp0 closing || lamp1 fading | lamp0 fading | lamp1 on | lamp0 on


// The View Matrix
mat4 matrixView;

// Camera & navigation
float maxspeed = 4.f;	// camera max speed
float accel = 4.f;		// camera acceleration
vec3 _acc(0), _vel(0);	// camera acceleration and velocity vectors
float _fov = 60.f;		// field of view (zoom)

bool init()
{
	dematerializer = true;
	win_h = glutGet(GLUT_WINDOW_HEIGHT);
	win_w = glutGet(GLUT_WINDOW_WIDTH);
	//vector<Transform*> transforms = { &table_t, &chair0,
	//	&chair1, &chair2, &chair3, &vase_t, &chick_t,
	//	&lamp0, &lamp1, &teapot_t
	//};

	//TransformScene(vec3(0, 0, 0), vec3(0, -90, 0), vec3(1, 1, 1), transforms); //I realised I don t need to rotate each obj individually, but around a pivot point
	
	

	// Initialise Shaders & Programs
	float x = 0.7f;
	programs = {
		Program("basic.vert", "basic.frag",
			std::vector<Uniform>{
				Uniform("lightDir.direction",normalize(vec3(0.5f, 0.5f, 0.5f))),
				Uniform("lightDir.diffuse",vec3(5.0f, 1.0f, 0.5f)),
				Uniform("lightAmbient.color",vec3(x)),
				Uniform("materialAmbient",vec3(1.0f)),
				Uniform("materialDiffuse",vec3(0.2, 0.2, 0.6)),
				Uniform("materialSpecular", vec3(0.6, 0.6, 1.0)),
				Uniform("att_quadratic", 0.5f),
				Uniform("fogColour", vec3(1)),
				Uniform("fogDensity", 0.0f)
		}),
		Program("flag.vert", "flag.frag",
			std::vector<Uniform>{
				Uniform("materialAmbient", vec3(1.0f)),
				Uniform("lightAmbient.color", vec3(x))
		}),
		Program("post.vert", "post.frag",
			std::vector<Uniform>{
				Uniform("matrixProjection", ortho(0, 1, 0, 1, -1, 1))
		})/*,
		Program("reflect.vert", "reflect.frag",
		std::vector<Uniform>{
		})*/
			,
		Program("post1.vert", "post1.frag",
			std::vector<Uniform>{
				Uniform("matrixProjection", ortho(0, 1, 0, 1, -1, 1))
		}),
		Program("post2.vert", "post2.frag",
			std::vector<Uniform>{
				Uniform("matrixProjection", ortho(0, 1, 0, 1, -1, 1))
		})

	};
	programs[BASIC].use(true);

	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!


	// glut additional setup
	glutSetVertexAttribCoord3(programs[BASIC].getAttribLocation("aVertex"));
	glutSetVertexAttribNormal(programs[BASIC].getAttribLocation("aNormal"));
	glutSetVertexAttribTexCoord2(programs[BASIC].getAttribLocation("aTexCoord"));

	// load your 3D models here!
	ModelsInit(std::vector<std::string>{"table.obj", "vase.obj", "chicken.obj", "lamp.obj", "utah_teapot_hires.obj", "flag_plane.obj", "flag_pole.obj", "Aj.fbx"});

	

	// textures --- !!!ALWAYS LOAD IN ORDER OF "BITMAPS" ENUM!!!
	if (!AddNew2DTexture("oak.png")) return false;
	if (!AddNew2DTexture("cloth.bmp")) return false;
	if (!AddNew2DTexture("kingston_logo1k.jpg")) return false;
	if (!AddNew2DTexture("sand.png")) return false;
	if (!AddNew2DTexture("perlin.png")) return false;

	//initialising shadows
	shadows[0].Init(BASIC, 0);
	//shadows[1].Init(BASIC, 1);

	// adding post effects
	AddNewPost();
	AddNewPost();
	AddNewPost();

	MyAnimation::MakeMoveAnimConstSpeed(std::vector<Instruction>{
		{ {0, 1, 0}, -1},
		{ {-2, -2, 0}, -1 },
		{ {0, 1, 2}, -1 },
		{ {5, 0.5, 0}, -1 },
		{ {0, -1, -2}, 1 },
		{ {-3, 0.5, 0}, -1 }
	}, &lights[0].pos, 2, true);

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1), radians(12.f), vec3(1, 0, 0));
	matrixView *= lookAt(
		vec3(0.0, 5.0, 10.0),
		vec3(0.0, 5.0, 0.0),
		vec3(0.0, 1.0, 0.0));
	matrixView[0] = vec4(-0.584417f, -0.196366f, 0.787276f, 0.0f);
	matrixView[1] = vec4(0.0f, 0.97022f, 0.241998f, 0.0f);
	matrixView[2] = vec4(-0.811397f, 0.141439f, -0.567058f, 0.0f);
	matrixView[3] = vec4(-0.0590151f, -4.07038f, -8.12573f, 1.0f);

	// setup the screen background colour
	glClearColor(0.18f, 0.25f, 0.22f, 1.0f);   // deep grey background

	std::cout << std::endl;
	std::cout << "Use:" << std::endl;
	std::cout << "  WASD or arrow key to navigate" << std::endl;
	std::cout << "  QE or PgUp/Dn to move the camera up and down" << std::endl;
	std::cout << "  Shift to speed up your movement" << std::endl;
	std::cout << "  Drag the mouse to look around" << std::endl;
	std::cout << std::endl;

	return true;
}
void SendProgramSpecificUniforms(unsigned char prog_id) {
	switch (prog_id)
	{
	case 2:
		/*programs[prog_id].sendUniform("time", glutGet(GLUT_ELAPSED_TIME) * 0.001f);
		programs[prog_id].sendUniform("bools", Post0_bools);*/
		break;
	case 3:
		programs[prog_id].sendUniform("time", glutGet(GLUT_ELAPSED_TIME) * 0.001f);
		programs[prog_id].sendUniform("bools", Post0_bools);
		break;
	case 4:
		programs[prog_id].sendUniform("time", glutGet(GLUT_ELAPSED_TIME) * 0.001f);
		programs[prog_id].sendUniform("blur", blur);
		break;
	default:
		break;
	}
}

void SetUpLightBulb(unsigned char light, mat4 matrixView, _3dgl::C3dglProgram& prog = programs[BASIC], float radius = 0.1f) {
	prog.use(true);
	SetUpLight(light, matrixView, prog);
	matrixView = translate(matrixView, (vec3)lights[light].pos);
	matrixView = scale(matrixView, vec3(radius));
	prog.sendUniform("matrixModelView", matrixView);
	prog.sendUniform("receiveShadows", false);
	glutSolidSphere(1, 32, 32);
}
void SetUpLightBulb(unsigned char light, mat4 matrixView, float time, float radius = 0.1f, _3dgl::C3dglProgram & prog = programs[BASIC]) {
	prog.use(true);
	lights[light].ToggleProcess(time);
	SetUpLight(light, matrixView, prog);
	matrixView = translate(matrixView, (vec3)lights[light].pos);
	matrixView = scale(matrixView, vec3(radius));
	prog.sendUniform("matrixModelView", matrixView);
	prog.sendUniform("receiveShadows", false);
	glutSolidSphere(1, 32, 32);
}

void renderScene(mat4& matrixView, float time, float deltaTime)
{
	win_h = glutGet(GLUT_WINDOW_HEIGHT);
	win_w = glutGet(GLUT_WINDOW_WIDTH);
	//light bulbs
	if (ShadowPassing == false) {
		SetUpLightBulb(0, matrixView, time);
		SetUpLightBulb(1, matrixView, time);
	}
	SetUpModels(matrixView);
	
	programs[BASIC].use(true);

}
// called before window opened or resized - to setup the Projection Matrix
void onReshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = glm::perspective(radians(_fov), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	programs[BASIC].sendUniform("matrixProjection", matrixProjection);
	programs[FLAG].sendUniform("matrixProjection", matrixProjection);
}
void onRender()
{
	
	// these variables control time & animation
	static float prev = 0;
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;	// time since start in seconds
	float deltaTime = time - prev;						// time since last frame
	prev = time;										// framerate is 1/deltaTime
	Globals::global_time = time;
	win_h = glutGet(GLUT_WINDOW_HEIGHT);
	win_w = glutGet(GLUT_WINDOW_WIDTH);

	MyAnimation::Update(time, deltaTime);

	ShadowPassing = true;
	shadows[0].createShadowMap(glm::lookAt((vec3)lights[0].pos - vec3(0, 0, 0.1f), (vec3)lights[0].pos + (vec3)lights[0].dir, vec3(0, 1, 0)), time, deltaTime, renderScene, onReshape);
	//shadows[1].createShadowMap(glm::lookAt((vec3)lights[1].pos - vec3(0, 0, 0.1f), (vec3)lights[1].pos + (vec3)lights[1].dir, vec3(0, 1, 0)), time, deltaTime, renderScene, onReshape);
	ShadowPassing = false;
	glBindFramebufferEXT(GL_FRAMEBUFFER, idFBO[Posts[FIRST].fbo]);

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	_vel = clamp(_vel + _acc * deltaTime, -vec3(maxspeed), vec3(maxspeed));
	float pitch = _3dgl::getPitch(matrixView);
	matrixView = rotate(translate(rotate(mat4(1),
		pitch, vec3(1, 0, 0)),	// switch the pitch off
		_vel * deltaTime),		// animate camera motion (controlled by WASD keys)
		-pitch, vec3(1, 0, 0))	// switch the pitch on
		* matrixView;

	// render the scene objects
	renderScene(matrixView, time, deltaTime);


	//Post
	SendProgramSpecificUniforms(POST0);
	SendProgramSpecificUniforms(POST1);
	SendProgramSpecificUniforms(POST2);
	ApplyPosts();
	onReshape(win_w, win_h);
	programs[BASIC].use(true);

	//End Post

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}



// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': _acc.z = accel; break;
	case 's': _acc.z = -accel; break;
	case 'a': _acc.x = accel; break;
	case 'd': _acc.x = -accel; break;
	case 'e': _acc.y = accel; break;
	case 'q': _acc.y = -accel; break;
	/*case 'm':
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				std::cout << matrixView[j][i] << "(" << j << "," << i << ") ";
			}
			std::cout << "\n";
		}
		break;*/
	}
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': _acc.z = _vel.z = 0; break;
	case 'a':
	case 'd': _acc.x = _vel.x = 0; break;
	case 'q': //std::cout << "\nX: " << test.x << "\nY: " << test.y << "\nZ: " << test.z << "\n\n";
	case 'e': _acc.y = _vel.y = 0; break;
	case '1': lights[0].Toggle(); break;
	case '2': lights[1].Toggle(); break;
		/*case '0': test.z -= 0.02f; break;
		case '9': test.z += 0.02f; break;
		case '8': test.y -= 0.02f; break;
		case '7': test.y += 0.02f; break;
		case '6': test.x -= 0.02f; break;
		case '5': test.x += 0.02f; break;*/
	case 'c': Post0_bools ^= 0x1; break;
	case 'v': Post0_bools ^= 0x2; break;
	case 'b': Post0_bools ^= 0x4; break;
	case 'n': Post0_bools ^= 0x8; break;
	case 'm': blur = !blur; break;
	case 'z': models[M_Aj].p.currentAnimation++; break;
	case 'x': models[M_Aj].p.currentAnimation--; break;
	case 'k': MyAnimation::activated = !MyAnimation::activated; break;
	case 'l': dematerializer = !dematerializer;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
void onMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		mouse_first_grab = true;
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	}
	else {
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
	if (button == 1)
	{
		_fov = 60.0f;
		onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

// handle mouse move
void onMotion(int x, int y)
{
	if (mouse_first_grab) {
		mouse_first_grab = false;
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		return;
	}
	int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

	// find delta (change to) pan & pitch
	float deltaYaw = 0.005f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
	float deltaPitch = 0.005f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

	if (abs(deltaYaw) > 0.3f || abs(deltaPitch) > 0.3f) {
		glutWarpPointer(centerX, centerY);
		return;	// avoid warping side-effects
	}

	// View = Pitch * DeltaPitch * DeltaYaw * Pitch^-1 * View;
	constexpr float maxPitch = radians(80.f);
	float pitch = _3dgl::getPitch(matrixView);
	float newPitch = glm::clamp(pitch + deltaPitch, -maxPitch, maxPitch);
	matrixView = rotate(rotate(rotate(mat4(1.f),
		newPitch, vec3(1.f, 0.f, 0.f)),
		deltaYaw, vec3(0.f, 1.f, 0.f)),
		-pitch, vec3(1.f, 0.f, 0.f))
		* matrixView;
	glutWarpPointer(centerX, centerY);
}

void onMouseWheel(int button, int dir, int x, int y)
{
	_fov = glm::clamp(_fov - dir * 5.f, 5.0f, 175.f);
	onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

int main(int argc, char** argv)
{

	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("3DGL Scene: First Example");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		_3dgl::C3dglLogger::log("GLEW Error {}", (const char*)glewGetErrorString(err));
		return 0;
	}
	_3dgl::C3dglLogger::log("Using GLEW {}", (const char*)glewGetString(GLEW_VERSION));

	// register callbacks
	glutDisplayFunc(onRender);
	glutReshapeFunc(onReshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutMouseWheelFunc(onMouseWheel);

	_3dgl::C3dglLogger::log("Vendor: {}", (const char*)glGetString(GL_VENDOR));
	_3dgl::C3dglLogger::log("Renderer: {}", (const char*)glGetString(GL_RENDERER));
	_3dgl::C3dglLogger::log("Version: {}", (const char*)glGetString(GL_VERSION));
	_3dgl::C3dglLogger::log("");

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{

		_3dgl::C3dglLogger::log("Application failed to initialise\r\n");
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

