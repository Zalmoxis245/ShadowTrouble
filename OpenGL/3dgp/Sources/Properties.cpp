#include "Properties.h"


Properties::Properties(float m_shininess, glm::vec3 m_colour, Transform m_transform, unsigned char _uniqueModel_id, bool _animated, unsigned char m_program_id,
	const std::vector<SeparateAnimation>& _separate_animations, char m_model_render_id,
	bool m_hasTex, unsigned char m_tex_id, const std::vector<glm::vec3>& m_assp_vec3, const std::vector<float>& m_assp_float) {
	colour = m_colour;
	shininess = m_shininess;
	hasTex = m_hasTex;
	tex_id = m_tex_id;
	model_render_id = m_model_render_id;
	transform = m_transform;
	program_id = m_program_id;
	assp_vec3 = m_assp_vec3;
	assp_float = m_assp_float;
	uniqueModel_id = _uniqueModel_id;
	animated = _animated;
	separateAnimations = _separate_animations;
	currentAnimation = 0;
}

Properties::Properties() {
	colour = { 0, 0, 0 };
	shininess = 0.0f;
	hasTex = false;
	tex_id = 0;
	model_render_id = -1;
	program_id = 0;
	uniqueModel_id = 0;
	animated = false;
	currentAnimation = 0;
}