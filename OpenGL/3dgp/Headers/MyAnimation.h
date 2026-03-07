#pragma once
#include "Transform.h"
#include "Light.h"
#include <algorithm>

//start - target
//duration
//loops nr //-1 for no looping
struct PathWay {
	glm::vec3 start, target;
	float duration;
	char loops;
	PathWay(glm::vec3 start_, glm::vec3 target_, float duration_, unsigned char loops_);
};
//vector relative to last position
//duration of animation
//-1 for no loops
struct Instruction {
	MyVec3 amount;
	float duration;
	char loops; 
};

class MyAnimation {
	MyVec3* object;
	std::vector<PathWay> paths;
	float elapsedTime;
	unsigned int total_loops;
	unsigned char bools;//loop_whole_Animation | NULL | isLoopingBackwards | finished
	unsigned char currentPath, currentLoops;
	

public:
	MyAnimation();

	static std::vector<std::unique_ptr<MyAnimation>> animations;
	static bool activated;
	static void Update(float time, float deltaTime);
	void Move(float time, float deltaTime);
	static void MakeMoveAnim(const std::vector<Instruction> instructions, MyVec3* _object, bool whole_animation_loop);
	static void MakeMoveAnimConstSpeed(const std::vector<Instruction> instructions, MyVec3* _object, float speed, bool whole_animation_loop);
};