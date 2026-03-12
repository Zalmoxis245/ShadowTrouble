#include "Textures.h"

short Textures::cnt_gl_texture_id_count = 1;
short Textures::cnt_gl_texture0_id = 0;
std::vector<GLuint> Textures::idTex;
std::vector<_3dgl::C3dglBitmap> Textures::textures;
//Use in init like this:
//if (!Textures::AddNew2DTexture("oak.png")) return false;
bool Textures::AddNew2DTexture(std::string path) {//if true -> do nothing else -> return false in init
	textures.push_back(_3dgl::C3dglBitmap());
	textures.back().load("models/" + path, GL_RGBA);
	if (!textures.back().getBits()) return false;
	
	idTex.push_back(GLuint());
	glGenTextures(1, &idTex.back());
	glBindTexture(GL_TEXTURE_2D, idTex.back());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures.back().getWidth(), textures.back().getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textures.back().getBits());
	glGenerateMipmap(GL_TEXTURE_2D);
	return true;
}