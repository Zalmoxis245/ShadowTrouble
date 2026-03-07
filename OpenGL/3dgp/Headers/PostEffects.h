#pragma once
#include "Globals.h"
#include <GL/glew.h>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include <vector>
#include "Textures.h"
#include "Program.h"

enum POSTS : unsigned char {
	FIRST,
	SECOND,
	THIRD
};
class PostEffects {
public:
	GLuint id;
	short tex_id;
	unsigned char fbo, depth, vbo, prog; //255==error 254 ==ignore 253 == max capacity error
	bool fullscreen;

	PostEffects();
	static std::vector<PostEffects> Posts;
	static std::vector<GLuint> idFBO;
	static std::vector<GLuint> idDepthBuffer;
	static std::vector<GLuint> idVBO;

	static void AddNewPost(bool FBO, bool DepthBuffer, bool fullscreen, short width = Globals::win_w, short height = Globals::win_h);
	static void AddNewPost();
	static void DrawFullscreenQuad(GLuint attribVertex, GLuint attribTextCoord);
	static void ApplyPosts();
};