#include "Cube.h"

_3dgl::C3dglSkyBox Cube::skybox;



bool Cube::InitSkybox(std::string front, std::string left, std::string back, std::string right, std::string up, std::string down) {
	if (!skybox.load(("models\\" + front).c_str(), 
		("models\\" + left).c_str(), 
		("models\\" + back).c_str(), 
		("models\\" + right).c_str(), 
		("models\\" + up).c_str(), 
		("models\\" + down).c_str()))	
		return false;
	return true;
}