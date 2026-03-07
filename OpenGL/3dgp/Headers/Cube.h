#pragma once
#include "Globals.h"
#include <GL/glew.h>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include <vector>
#include <iostream>

#include "Program.h"


struct Cube {
	GLuint tex = 0; 
	unsigned char tex_id = 0, prog;
	void Init(short& free_gl_tex, unsigned char program) {
		tex_id = (unsigned char)free_gl_tex++;
		prog = program;
	}

	void prepareCubeMap(float x, float y, float z, float time, float deltaTime, glm::mat4& matrixView, void (*renderScene)(glm::mat4&, float, float))
	{
		// Store the current viewport in a safe place
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		int w = viewport[2];
		int h = viewport[3];
		// setup the viewport to 256x256, 90 degrees FoV (Field of View)
		glViewport(0, 0, 256, 256);
		Program::programs[prog].sendUniform("matrixProjection", glm::perspective(glm::radians(90.f), 1.0f, 0.02f, 1000.0f));
		// render environment 6 times
		Program::programs[prog].sendUniform("reflectionPower", 0.0);
		for (int i = 0; i < 6; ++i)
		{
			// clear background
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// setup the camera
			const GLfloat ROTATION[6][6] =
			{ // at up
			{ 1.0, 0.0, 0.0, 0.0, -1.0, 0.0 }, // pos x
			{ -1.0, 0.0, 0.0, 0.0, -1.0, 0.0 }, // neg x
			{ 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 }, // pos y
			{ 0.0, -1.0, 0.0, 0.0, 0.0, -1.0 }, // neg y
			{ 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 }, // poz z
			{ 0.0, 0.0, -1.0, 0.0, -1.0, 0.0 } // neg z
			};
			glm::mat4 matrixView2 = glm::lookAt(
				glm::vec3(x, y, z),
				glm::vec3(x + ROTATION[i][0], y + ROTATION[i][1], z + ROTATION[i][2]),
				glm::vec3(ROTATION[i][3], ROTATION[i][4], ROTATION[i][5]));
			// send the View Matrix
			Program::programs[prog].sendUniform("matrixView", matrixView);
			// render scene objects - all but the reflective one
			glActiveTexture(GL_TEXTURE0);
			renderScene(matrixView2, time, deltaTime);
			// send the image to the cube texture
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
			glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 0, 0, 256, 256, 0);
		}
		// restore the matrixView, viewport and projection
		void onReshape(int w, int h);
		onReshape(w, h);
	}

	static _3dgl::C3dglSkyBox skybox;
	static bool InitSkybox(std::string front, std::string left, std::string back, std::string right, std::string up, std::string down);
};