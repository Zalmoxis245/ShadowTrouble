#pragma once
#include "Transform.h"
#include <3dgl/3dgl.h>
#include <vector>

struct SeparateAnimation {
	_3dgl::C3dglModel* model = nullptr;
	std::string path;
};

struct Properties {
	Transform transform;
	glm::vec3 colour;
	std::vector<glm::vec3> assp_vec3; //additional shader specific properties - ASSP
	std::vector<float> assp_float;
	std::vector<SeparateAnimation> separateAnimations; //in case it has no animations on itself
	float shininess;
	bool hasTex, animated;
	unsigned char tex_id, program_id, currentAnimation;
	char model_render_id, uniqueModel_id;

	//Shininess - Colour - Transform - Unique Model
	//Animated - Program - Separate Animations - Model Render ID
	//Has Texture - GL_TEX_ID - Shader Specific vec3s - Shader Specific floats
	Properties(float m_shininess, glm::vec3 m_colour, Transform m_transform, unsigned char _uniqueModel_id, bool _animated ,unsigned char m_program_id = 0,
		const std::vector<SeparateAnimation>& _separate_animations = {}, char m_model_render_id = -1,
		bool m_hasTex = false, unsigned char m_tex_id = 0, const std::vector<glm::vec3>& m_assp_vec3 = {}, const std::vector<float>& m_assp_float = {});
	Properties();
};