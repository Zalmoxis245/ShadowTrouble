#pragma once
#include <vector>
#ifndef GLM_FORCE_INLINE
#define GLM_FORCE_INLINE
#endif

#include <GL/glew.h>
#include <GL/glut.h>
#include "../glm/glm.hpp"
#include "Program.h"

struct MyVec3 {
    float x, y, z;

	MyVec3() : x(0), y(0), z(0) {}
    MyVec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	MyVec3(float f) : x(f), y(f), z(f) {}
    MyVec3(const glm::vec3& v) : x(v.x), y(v.y), z(v.z) {}
	MyVec3(const glm::vec4& v) : x(v.x), y(v.y), z(v.z) {}
    MyVec3 operator-(const MyVec3& other) const { return MyVec3(x - other.x, y - other.y, z - other.z); }
    MyVec3 operator+(const MyVec3& other) const { return MyVec3(x + other.x, y + other.y, z + other.z); }
    MyVec3 operator*(float f) const { return MyVec3(x * f, y * f, z * f); }
    MyVec3 operator/(float f) const { return MyVec3(x / f, y / f, z / f); }
    operator glm::vec3() const { return glm::vec3(x, y, z); }
	operator glm::vec4() const { return glm::vec4(x, y, z, 1.0f); }
};
struct MyVec4 {
    float x, y, z, w;
    MyVec4() : x(0), y(0), z(0), w(0) {}
    MyVec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    MyVec4(float f) : x(f), y(f), z(f), w(f) {}
    MyVec4(const MyVec3& v, float _w = 1.0f) : x(v.x), y(v.y), z(v.z), w(_w) {}
    MyVec4(const glm::vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    MyVec4(const glm::vec3& v, float _w = 1.0f) : x(v.x), y(v.y), z(v.z), w(_w) {}
    MyVec4 operator-(const MyVec4& other) const { return MyVec4(x - other.x, y - other.y, z - other.z, w - other.w); }
    MyVec4 operator+(const MyVec4& other) const { return MyVec4(x + other.x, y + other.y, z + other.z, w + other.w); }
    MyVec4 operator*(float f) const { return MyVec4(x * f, y * f, z * f, w * f); }
    MyVec4 operator/(float f) const { return MyVec4(x / f, y / f, z / f, w / f); }
    operator glm::vec4() const { return glm::vec4(x, y, z, w); }
    operator glm::vec3() const { return glm::vec3(x, y, z); } 
    operator MyVec3() const { return MyVec3(x, y, z); }      
};
inline float dot(const MyVec3& a, const MyVec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float dot(const MyVec4& a, const MyVec4& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
inline float length(const MyVec3& a) { return sqrt(dot(a, a)); }
inline float length(const MyVec4& a) { return sqrt(dot(a, a)); }
//MyVec3
inline glm::vec3 operator+(const MyVec3& a, const glm::vec3& b) { return glm::vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline glm::vec3 operator+(const glm::vec3& a, const MyVec3& b) { return glm::vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline glm::vec3 operator-(const MyVec3& a, const glm::vec3& b) { return glm::vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline glm::vec3 operator-(const glm::vec3& a, const MyVec3& b) { return glm::vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
//MyVec4
inline MyVec4 operator+(const MyVec4& a, const MyVec3& b) { return MyVec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w); }
inline MyVec4 operator+(const MyVec3& a, const MyVec4& b) { return MyVec4(a.x + b.x, a.y + b.y, a.z + b.z, b.w); }
inline glm::vec4 operator+(const MyVec4& a, const glm::vec4& b) { return glm::vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
inline glm::vec4 operator-(const MyVec4& a, const glm::vec4& b) { return glm::vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
inline glm::vec4 operator*(const glm::mat4& m, const MyVec4& v) {return m * glm::vec4(v); }



struct Light {
	MyVec3 pos, dir;
	MyVec3 diffuse, specular;
	float intensity, startIntensity, targetInt;
	float cutoff, exponent;
	float fadeStart, fadeDuration;
	unsigned char bools; //NULL | NULL | NULL | NULL || NULL | closing | fading | on

public:
	static std::vector<Light> MyLights;
	Light(unsigned char m_bools, MyVec3 m_pos, MyVec3 m_dir, float m_intensity, float m_startIntensity, float m_fadeDuration, float m_targetInt = 1.0f,
		MyVec3 m_diffuse = MyVec3(1.0f), float m_cutoff = cos(glm::radians(30.0f)), MyVec3 m_specular = glm::vec3(1.0f), float m_exponent = 10.0f);
	Light();
	void ToggleProcess(float time);
	void Toggle();
	static MyVec3 GetAmbient(MyVec3 world_pos, MyVec3 baseAmbient = MyVec3(0.2f));
	static float GetWorldAmbience();
	
	static void SetUpLight(unsigned char light, glm::mat4 matrixView, _3dgl::C3dglProgram& prog = Program::programs[BASIC]);
};