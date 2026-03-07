#include "Light.h"

std::vector<Light> Light::MyLights = { Light(
		0x1, //bools
		{ -0.692f, 4.984f, -1.312f}, //pos
		{0.0f, -1.0f, 0.0f}, //dir
		1.0f, //intensity
		2.0f, //fade duration
		1.0f, //start intensity
		1.0f, //target intesity
		{5.0f, 5.0f, 5.0f}, //diffuse
		0.8660254f, //cutoff 30 degrees ; cos(radians(30));
		{10.0f, 10.0f, 10.0f}, //specular
		2.0f //exponent
	),
	Light(
		0x1,
		{3.231f, 4.984f, 1.501f},
		{0.0f, -1.0f, 0.0f},
		1.0f, //intensity
		2.0f, //fade duration
		1.0f, //start intensity
		1.0f, //target intesity
		{5.0f, 5.0f, 5.0f}, //diffuse
		0.8660254f, //cutoff
		{10.0f, 10.0f, 10.0f}, //specular
		2.0f //exponent
	)
};

Light::Light(unsigned char m_bools, MyVec3 m_pos, MyVec3 m_dir, float m_intensity, float m_startIntensity, float m_fadeDuration, float m_targetInt,
	MyVec3 m_diffuse, float m_cutoff, MyVec3 m_specular, float m_exponent) {
	pos = m_pos;
	dir = m_dir;

	diffuse = m_diffuse;
	specular = m_specular;

	intensity = m_intensity;
	startIntensity = m_startIntensity;
	fadeDuration = m_fadeDuration;
	targetInt = m_targetInt;

	cutoff = m_cutoff;
	exponent = m_exponent;
	fadeStart = 0;
	bools = m_bools;
}
Light::Light() {
	pos = { 0, 0, 0 }; // NEVER USE glm::vec3() inside of non-main files like this cuz Linker error (especially inside constructors)
	dir = { 0, 0, 0 }; // Actually just declaring glm::vec3 is fine. Problem appears on assignemnt. glm::vec3 a; is fine but if you do "a = vec3(1);" then error

	diffuse = { 1, 1, 1 };
	specular = { 1, 1, 1 };

	intensity = 1.0f;
	startIntensity = 1.0f;
	fadeDuration = 2.0f;
	targetInt = 1.0f;

	cutoff = 0.15425145f;
	exponent = 10.0f;
	fadeStart = 0;
	bools = 0x1;
}
void Light::ToggleProcess(float time)
{
	if (!(bools & 0x2)) return;
	float t = glm::clamp((time - fadeStart) / fadeDuration, 0.0f, 1.0f);
	intensity = glm::mix(startIntensity, targetInt, t);
	if (t >= 1.0f) bools &= ~0x2;
}
void Light::Toggle() {
	bools ^= 1; bools |= 6;
	fadeStart = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	startIntensity = intensity;
	targetInt = (bools & 0x1) ? 1.0f : 0.0f;
}

MyVec3 Light::GetAmbient(MyVec3 world_pos, MyVec3 baseAmbient) {
	float activeLights = 0.0f;
	for (Light& i : MyLights) activeLights += i.intensity;
	float lightFactor = glm::clamp(activeLights / 2.0f, 0.0f, 1.0f);
	float ambientFactor = 0.0f;
	for (Light& i : MyLights) ambientFactor += (1.0f / (1.0f + length(i.pos - world_pos))) * i.intensity;
	ambientFactor = glm::clamp(ambientFactor, 0.2f, 1.0f);


	return baseAmbient * ambientFactor * lightFactor;
}
float Light::GetWorldAmbience() {
	float activeLights = 0.0f;
	for (Light& i : MyLights) activeLights += i.intensity;
	float lightFactor = glm::clamp(activeLights / 2.0f, 0.0f, 1.0f);
	float ambientFactor = 0.0f;
	for (Light& i : MyLights) ambientFactor += (1.0f / (1.0f + length(i.pos))) * i.intensity;
	ambientFactor = glm::clamp(ambientFactor, 0.2f, 1.0f);

	return ambientFactor * lightFactor;
}

void Light::SetUpLight(unsigned char light, glm::mat4 matrixView, _3dgl::C3dglProgram& prog) {
	prog.use(true);
	prog.sendUniform("hasTex", false);
	MyVec3 temp;

	//light point position
	temp = MyVec3(matrixView * MyVec4(MyLights[light].pos, 1.0));
	prog.sendUniform("lightPoint" + std::to_string((int)light) + ".position", temp.x, temp.y, temp.z);
	//light spot direction
	temp = MyVec3(matrixView * MyVec4(MyLights[light].dir, 0.0));
	prog.sendUniform("lightSpot" + std::to_string((int)light) + ".direction", temp.x, temp.y, temp.z);
	//light spot floats
	prog.sendUniform("lightSpot" + std::to_string((int)light) + ".cutoff", MyLights[light].cutoff);
	prog.sendUniform("lightSpot" + std::to_string((int)light) + ".exponent", MyLights[light].exponent);
	prog.sendUniform("shadowLightIntensity[" + std::to_string((int)light) + "]", MyLights[light].intensity);
	//light point diffuse
	temp = (MyLights[light].bools & 5) ? MyLights[light].diffuse * MyLights[light].intensity : MyVec3(0.0);
	prog.sendUniform("lightPoint" + std::to_string((int)light) + ".diffuse", temp.x, temp.y, temp.z);
	//light point specular
	temp = (MyLights[light].bools & 5) ? MyLights[light].specular * MyLights[light].intensity : MyVec3(0.0);
	prog.sendUniform("lightPoint" + std::to_string((int)light) + ".specular", temp.x, temp.y, temp.z);
	//ambient
	temp = MyVec3((MyLights[light].intensity + Light::GetWorldAmbience() * 2.0f));
	prog.sendUniform("materialAmbient",  temp.x, temp.y, temp.z);

	
}
