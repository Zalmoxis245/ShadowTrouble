#include "Transform.h"


Transform::Transform(glm::vec3 m_pos, glm::vec3 m_rot, glm::vec3 m_scale) {
	pos = m_pos;
	rot = m_rot;
	scale = m_scale;
}
Transform::Transform() {
	pos = glm::vec3(0, 0, 0);
	rot = glm::vec3(0, 0, 0);
	scale = glm::vec3(0, 0, 0);
}

void Transform::print() {
	std::cout << "/t///Position:/t///" << "\n" << pos.x << "\n" << pos.y << "\n" << pos.z << "\n---\t---\t---\n";
	std::cout << "/t///Rotation:/t///" << "\n" << rot.x << "\n" << rot.y << "\n" << rot.z << "\n---\t---\t---\n";
	std::cout << "/t///Scale:/t///" << "\n" << scale.x << "\n" << scale.y << "\n" << scale.z << "\n---\t---\t---\n";
}

//void TransformScene(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, std::vector<Transform*>& objects) {
//	for (Transform* t : objects) {
//		t->pos += translation;
//		t->rot += rotation;
//		t->scale *= scale;
//	}
//}