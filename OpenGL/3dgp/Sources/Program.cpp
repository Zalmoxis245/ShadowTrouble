#include "Program.h"

std::vector<Program> Program::programs;

Uniform::Uniform(std::string u_name, UniformValue u_value) {
	name = u_name;
	value = u_value;
}
Program::Program(std::string vert_path, std::string frag_path, const std::vector<Uniform>& uniforms) {
	this->Init(vert_path, frag_path, uniforms);
}
void Program::Init(std::string vert_path, std::string frag_path, const std::vector<Uniform>& uniforms) {
	_3dgl::C3dglShader vertexShader;
	_3dgl::C3dglShader fragmentShader;

	if (!vertexShader.create(GL_VERTEX_SHADER)) std::cout << COUT_RED << "Error 010" << RESET;
	if (!vertexShader.loadFromFile("shaders/" + vert_path)) std::cout << COUT_RED << "Error 011" << RESET;
	if (!vertexShader.compile()) std::cout << COUT_RED << "Error 012" << RESET;

	if (!fragmentShader.create(GL_FRAGMENT_SHADER)) std::cout << COUT_RED << "Error 013" << RESET;
	if (!fragmentShader.loadFromFile("shaders/" + frag_path)) std::cout << COUT_RED << "Error 014" << RESET;
	if (!fragmentShader.compile()) std::cout << COUT_RED << "Error 015" << RESET;

	if (!this->create()) std::cout << COUT_RED << "Error 016" << RESET;
	if (!this->attach(vertexShader)) std::cout << COUT_RED << "Error 017" << RESET;
	if (!this->attach(fragmentShader)) std::cout << COUT_RED << "Error 018" << RESET;
	if (!this->link()) std::cout << COUT_RED << "Error 019" << RESET;
	if (!this->use(true)) std::cout << COUT_RED << "Error 020" << RESET;

	this->use(true);
	for (const auto& u : uniforms) std::visit([&](auto&& val) { this->sendUniform(u.name, val); }, u.value);
}