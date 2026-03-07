#pragma once
#include "Globals.h"
#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include "../glm/glm.hpp"
#include <vector>

enum BITMAPS : unsigned char {
	WOOD,
	CLOTH,
	FLAG_TEX,
	SAND
};

struct Textures {
	static short cnt_gl_texture_id_count;
	static std::vector<GLuint> idTex;
	static std::vector<_3dgl::C3dglBitmap> textures;

	static bool AddNew2DTexture(std::string path);
};