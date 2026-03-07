#pragma once
#include "../glm/glm.hpp"
#include <iostream>
#include <vector>

struct Transform {
	glm::vec3 pos, rot, scale;

	//Position - Rotation - Scale
	Transform(glm::vec3 m_pos, glm::vec3 m_rot, glm::vec3 m_scale);
	Transform();
	void print();
};