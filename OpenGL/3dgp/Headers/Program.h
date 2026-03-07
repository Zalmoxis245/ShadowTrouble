#pragma once
#include "Globals.h"
#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include "../glm/glm.hpp"

#include <vector>
#include <variant>


typedef std::variant<int, float, glm::vec3, glm::mat4> UniformValue;
struct Uniform {
	std::string name;
	UniformValue value;
	Uniform(std::string u_name, UniformValue u_value);
};
enum PROGRAMS : unsigned char {
	BASIC,
	FLAG,
	POST0,
	POST1,
	POST2
};
class Program : public _3dgl::C3dglProgram{
public:
	static std::vector<Program> programs;
	Program(std::string vert_path, std::string frag_path, const std::vector<Uniform>& uniforms);
	void Init(std::string vert_path, std::string frag_path, const std::vector<Uniform>& uniforms);
};
