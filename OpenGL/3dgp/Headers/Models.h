#pragma once
#include <iostream>
#include <vector>
#include <math.h>


#include <3dgl/3dgl.h>
#include "../glm/gtc/matrix_transform.hpp"
#include "Transform.h"
#include "Properties.h"
#include "Light.h"
#include "Textures.h"


enum MODELS : unsigned char {
	M_TABLE, M_CHAIR0, M_CHAIR1, M_CHAIR2, M_CHAIR3, M_VASE, M_CHICK, M_LAMP0, M_LAMP1, M_TEAPOT, M_FLAG, M_POLE, M_Aj
};
enum UNIQUE_MODELS : unsigned char {
	UM_TABLE, UM_VASE, UM_CHICK, UM_LAMP, UM_TEAPOT, UM_FLAG, UM_POLE, UM_Aj
};

struct Models {
	_3dgl::C3dglModel* m;
	Properties p;

	Models(Properties _p);

	static std::vector<Models> models;
	static std::vector<_3dgl::C3dglModel> unique_models;

	static void SetUpMatrix(glm::mat4& m, Transform model);
	static void SendProgramSpecificUniforms(Models& m, glm::mat4 matrixView, _3dgl::C3dglProgram& prog);
	//static void SetUpModel(Models& m, glm::mat4 matrixView, _3dgl::C3dglModel& model, _3dgl::C3dglProgram& prog = Program::programs[BASIC]);
	static void SetUpModels(glm::mat4 matrixView);
	static bool ModelsInit(std::vector<std::string> paths);

	~Models();
};