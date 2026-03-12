#include "Models.h"

std::vector<Models> Models::models;
std::vector<_3dgl::C3dglModel> Models::unique_models;

Models::Models(Properties _p) { //I m not gonna configure it with pointers rn. Especially since this will only be called once in init most likely.
	p = _p;
	m = &unique_models[p.uniqueModel_id];
}

void Models::SetUpMatrix(glm::mat4& m, Transform model) {
	m = translate(m, model.pos);
	m = rotate(m, glm::radians(model.rot.x), glm::vec3(1.0f, 0.0f, 0.0f)); //glm::vec3 works here because it is in a static function
	m = rotate(m, glm::radians(model.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	m = rotate(m, glm::radians(model.rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	m = scale(m, model.scale);
}

void Models::SendProgramSpecificUniforms(Models& m, glm::mat4 matrixView, _3dgl::C3dglProgram& prog) {
	Properties& p = m.p;
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	switch (p.program_id)
	{
	case 0:
		prog.sendUniform("matrixView", matrixView);
		prog.sendUniform("materialAmbient", (glm::vec3)Light::GetAmbient(p.transform.pos, p.colour) * glm::vec3(3));
		prog.sendUniform("shininess", p.shininess);
		prog.sendUniform("animated", p.animated);
		prog.sendUniform("hasTex", p.hasTex);
		prog.sendUniform("receiveShadows", true);
		if (p.animated) {
			std::vector<glm::mat4> transforms;
			/*std::vector<glm::mat4> transformsIdle;
			std::vector<glm::mat4> transformsRun;
			std::vector<glm::mat4> mixed;
			float v = 1.0f;
			float thresh = 2.0f;
			float f = glm::clamp(v / thresh, 0.0f, 1.0f);

			m.m->getAnimData(0, time * 0.3f, transformsIdle);
			m.m->getAnimData(1, time * 0.3f, transformsRun);

			if (transformsIdle.size() == transformsRun.size()) {
				mixed.resize(transformsIdle.size());
				for (int i = 0; i < transformsIdle.size(); i++) {
					mixed[i] = (1.0f - f) * transformsIdle[i] + f * transformsRun[i];
				}
				prog.sendUniform("bones", &mixed[0], mixed.size());
			}*/

			unsigned char cAnim = m.p.currentAnimation;
			m.m->getAnimData((int)m.p.currentAnimation, time*0.3f, transforms);
			prog.sendUniform("bones", &transforms[0], transforms.size());
			
		}
		break;
	case 1:
		prog.sendUniform("materialAmbient", (glm::vec3)Light::GetAmbient(p.transform.pos, p.colour) * glm::vec3(3));
		prog.sendUniform("t", time);


		prog.sendUniform("freq", p.assp_float.at(0));
		prog.sendUniform("amp", p.assp_float.at(1));
		prog.sendUniform("speed", p.assp_float.at(2));
		prog.sendUniform("light.position", p.assp_vec3.at(0));
		prog.sendUniform("light.colour", p.assp_vec3.at(2));
		break;
	case 2:
		prog.sendUniform("time", time);
		break;
	default:
		break;
	}
}
//void Models::SetUpModel(Models& m, glm::mat4 matrixView, _3dgl::C3dglModel& model, _3dgl::C3dglProgram& prog) {
//	Properties& properties = m.p;
//	prog.use(true);
//	Models::SendProgramSpecificUniforms(m, matrixView, prog);
//	//Dematerializer
//	prog.sendUniform("dematerializer", 4); //this is temp
//	prog.sendUniform("t_dem", std::cosf(Globals::global_time) / 2 + 0.5);
//	//Dematerializer
//	if (properties.hasTex) {
//		prog.sendUniform("texture0", properties.tex_id);
//		glActiveTexture(GL_TEXTURE0 + properties.tex_id);
//		glBindTexture(GL_TEXTURE_2D, Textures::idTex[properties.tex_id]);
//	}
//	SetUpMatrix(matrixView, properties.transform);
//	prog.sendUniform("matrixModelView", matrixView);
//	//if (model.getName() == teapot.getName())glCullFace(GL_CW);
//	if (properties.model_render_id == -1) model.render(matrixView);
//	else model.render(properties.model_render_id, matrixView);
//	//if (model.getName() == teapot.getName())glCullFace(GL_CCW);
//}

void Models::SetUpModels(glm::mat4 matrixView) {
	
	for (Models& m : models) {
		glActiveTexture(GL_TEXTURE0);
		Program& prog = Program::programs[m.p.program_id];
		Properties& properties = m.p;
		_3dgl::C3dglModel* model = m.m;
		prog.use(true);
		Models::SendProgramSpecificUniforms(m, matrixView, prog);
		//Dematerializer
		prog.sendUniform("dematerializer", 7); //this is temp
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, Textures::idTex[4]);
		glActiveTexture(GL_TEXTURE0);
		prog.sendUniform("dematerializerActive", Globals::dematerializer);
		//Dematerializer
		prog.sendUniform("t_dem", std::abs(std::sin(glutGet(GLUT_ELAPSED_TIME) * 0.001f * 0.5f))-0.1f);
		//Dematerializer
		if (properties.hasTex) {
			prog.sendUniform("texture0", 0);
			glBindTexture(GL_TEXTURE_2D, Textures::idTex[properties.tex_id]);
		}
		glm::mat4 mod = matrixView;
		SetUpMatrix(mod, properties.transform);
		prog.sendUniform("matrixModelView", mod);
		//if (model.getName() == teapot.getName())glCullFace(GL_CW);
		
		if (properties.model_render_id == -1) model->render(mod);
		else model->render(properties.model_render_id, mod);
		//if (model.getName() == teapot.getName())glCullFace(GL_CCW);
	}
}

bool Models::ModelsInit(std::vector<std::string> paths) {
	unique_models.clear();
	models.clear();
	for (auto& path : paths) unique_models.emplace_back();


	//MODELS INIT
	models.emplace_back(Properties( //TABLE
		10.0f, //shininess
		glm::vec3(0.9f), //colour
		Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f)),
		UM_TABLE,
		false, //animated
		BASIC, //prog id
		{},
		1, //model render id
		true,//has texture
		WOOD
	));
	models.emplace_back(Properties( //CHAIR0
		10.0f, //shininess
		glm::vec3(0.9f), //colour
		Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f)),
		UM_TABLE,
		false,
		BASIC,
		{},
		0,    //model render id
		true, //has texture
		CLOTH    //texture id
	));
	models.emplace_back(Properties( //CHAIR1
		10.0f, //shininess
		glm::vec3(0.9f), //colour
		Transform(glm::vec3(0, 0, 0), glm::vec3(0, 180, 0), glm::vec3(0.005f, 0.005f, 0.005f)),
		UM_TABLE,
		false,
		BASIC,
		{},
		0,    //model render id
		true, //has texture
		CLOTH    //texture id
	));
	models.emplace_back(Properties( //CHAIR2
		10.0f, //shininess
		glm::vec3(0.9f), //colour
		Transform(glm::vec3(0, 0, 0), glm::vec3(0, 90, 0), glm::vec3(0.005f, 0.005f, 0.005f)),
		UM_TABLE,
		false,
		BASIC,
		{},
		0,    //model render id
		true, //has texture
		CLOTH    //texture id
	));
	models.emplace_back(Properties( //CHAIR3
		10.0f, //shininess
		glm::vec3(0.9f), //colour
		Transform(glm::vec3(0, 0, 0), glm::vec3(0, 270, 0), glm::vec3(0.005f, 0.005f, 0.005f)),
		UM_TABLE,
		false,
		BASIC,
		{},
		0,    //model render id
		true, //has texture
		CLOTH    //texture id
	));
	models.emplace_back(Properties( //VASE
		10.0f, //shininess
		glm::vec3(0.0f, 0.0f, 0.545f), //colour
		Transform(glm::vec3(0, 3.813f, 0), glm::vec3(0, 0, 0), glm::vec3(0.100f, 0.100f, 0.100f)),
		UM_VASE,
		false
	));
	models.emplace_back(Properties( //CHICK
		10.0f, //shininess
		glm::vec3(1.0f, 0.843f, 0.0f), //colour
		Transform(glm::vec3(1.819f, 5.213f, 0.495f), glm::vec3(0, 240.000f, 0), glm::vec3(0.020f, 0.020f, 0.020f)),
		UM_CHICK,
		false
	));
	models.emplace_back(Properties( //LAMP0
		10.0f, //shininess
		glm::vec3(0.8f, 0.5f, 0.0f), //colour
		Transform(glm::vec3(2.500f, 3.800f, 1.500f), glm::vec3(0, 180.000f, 0), glm::vec3(0.020f, 0.020f, 0.020f)),
		UM_LAMP,
		false
	));
	models.emplace_back(Properties( //LAMP1
		10.0f, //shininess
		glm::vec3(0.8f, 0.5f, 0.0f), //colour
		Transform(glm::vec3(-1.423f, 3.800f, -1.313f), glm::vec3(0, 180.000f, 0), glm::vec3(0.020f, 0.020f, 0.020f)),
		UM_LAMP,
		false
	));
	models.emplace_back(Properties( //TEAPOT
		10.0f, //shininess
		glm::vec3(0.0f, 0.8196f, 0.0f), //colour
		Transform(glm::vec3(-1.742f, 3.801f, 0), glm::vec3(0, 0, 0), glm::vec3(0.500f, 0.500f, 0.500f)),
		UM_TEAPOT,
		false
	));
	models.emplace_back(Properties( //FLAG
		10.0f, //shininess
		glm::vec3(0.9f), //colour
		Transform(glm::vec3(-3.431f, 5.986f, -1.832f), glm::vec3(0, 0, -90.0f), glm::vec3(2.000f, 1.0f, 3.0f)),
		UM_FLAG,
		false,
		FLAG,
		{},
		-1,
		true, //has texture
		FLAG_TEX,
		{ glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f) },//light related
			{
			8.0f, //freq
			0.3f, //amp
			5.0f //speed
			}
	));
	models.emplace_back(Properties( // FLAG POLE
		10.0f, //shininess
		glm::vec3(0.9f), //colour
		Transform(glm::vec3(-3.464f, 3.500f, -3.438f), glm::vec3(0), glm::vec3(0.100f, 4.000f, 0.100f)),
		UM_POLE,
		false,
		BASIC,
		{},
		-1,
		true, //has texture
		SAND
	));

	models.emplace_back(Properties( // Aj animated
		10.0f, //shininess
		glm::vec3(0.9f), //colour
		Transform(glm::vec3(0,0,5), glm::vec3(0), glm::vec3(0.05f)),
		UM_Aj,
		true,
		BASIC,
		{ {new _3dgl::C3dglModel(), "idle.fbx"}, {new _3dgl::C3dglModel(), "run.fbx"}, {new _3dgl::C3dglModel(), "walk.fbx"} },
		-1,
		false
	));


	//MODELS INIT END


	std::vector<unsigned char> seen;
	for (Models& m : models) {
		if (std::find(seen.begin(), seen.end(), m.p.uniqueModel_id) != seen.end()) continue;
		Program::programs[m.p.program_id].use(true);
		if (!unique_models[m.p.uniqueModel_id].load(("models\\" + paths[m.p.uniqueModel_id]).c_str())) return false;
		seen.push_back(m.p.uniqueModel_id);
	}
	
	for (Models& m : models) 
		if (m.p.animated) { 
			if (m.p.separateAnimations.empty()) m.m->loadAnimations(); 
			else {
				for (SeparateAnimation& sAnim : m.p.separateAnimations) {
					if (!sAnim.model->load(("models\\" + sAnim.path).c_str())) return false;
					m.m->loadAnimations(sAnim.model);
				}
			}
		}
	
	Program::programs[BASIC].use(true);
	return true;
}

Models::~Models() {
	for (auto& sAnim : p.separateAnimations) {
		if (sAnim.model != nullptr) {
			delete sAnim.model;     // Free the memory
			sAnim.model = nullptr;  // Prevent "dangling pointers"
		}
	}
}