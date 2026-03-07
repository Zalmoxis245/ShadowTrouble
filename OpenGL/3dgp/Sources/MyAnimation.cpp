#include "MyAnimation.h"

PathWay::PathWay(glm::vec3 start_, glm::vec3 target_, float duration_, unsigned char loops_) {
	start = start_;
	target = target_;
	duration = duration_;
	loops = loops_;

}

bool MyAnimation::activated = true;
std::vector<std::unique_ptr<MyAnimation>> MyAnimation::animations;


MyAnimation::MyAnimation() {
	object = nullptr;
	elapsedTime = 0;
	total_loops = 0;
	bools = 0;
	currentPath = 0, currentLoops = 0;
}

void MyAnimation::Update(float time, float deltaTime) {
	if (!activated) return;
	for (std::unique_ptr<MyAnimation>& anim : animations) {

		anim->Move(time, deltaTime);
	}
	animations.erase(std::remove_if(animations.begin(), animations.end(), [](const auto& anim) { return anim->bools & 0x1; }), animations.end());
}
void MyAnimation::Move(float time, float deltaTime) {

	if (!(paths[currentPath].duration > 0)) { 
		*object = paths[currentPath].target; 
		elapsedTime = 0;
		currentPath++;
		currentLoops = 0;
		if (currentPath >= paths.size()) bools |= 0x1;

		return;
	}
	
	elapsedTime += deltaTime;
	float t = std::clamp(elapsedTime / paths[currentPath].duration, 0.0f, 1.0f);
	if(bools&0x2) *object = glm::mix(paths[currentPath].target, paths[currentPath].start, t);
	else *object = glm::mix(paths[currentPath].start, paths[currentPath].target, t);

	if (t >= 1.0f) {
		elapsedTime = 0;
		if (bools & 0x2) { //if looping backwards
			*object = paths[currentPath].start;
			bools ^= 0x2;
		}
		else {
			*object = paths[currentPath].target;
			
			if (currentLoops >= paths[currentPath].loops) {
				currentPath++; currentLoops = 0;
				
			}
			else {
				bools ^= 0x2;
				currentLoops++;
			}
		}
		if (currentPath >= paths.size()) { //if finished
			if (bools & 0x8) {
				currentPath = 0;
				currentLoops = 0;
				bools = 8;
			}
			else {
				bools |= 0x1;
			}
		}
	}

}

void MyAnimation::MakeMoveAnim(const std::vector<Instruction> instructions, MyVec3* _object, bool whole_animation_loop) {
	animations.push_back(std::make_unique<MyAnimation>());

	animations.back()->total_loops = 0;
	MyVec3 currentPos = *_object;
	for (const Instruction& i : instructions) {
		animations.back()->paths.push_back(PathWay(currentPos, currentPos + i.amount, i.duration, i.loops));
		currentPos = currentPos + i.amount;
		animations.back()->total_loops += i.loops < 0 ? 1 : i.loops;
	}
	animations.back()->object = _object;
	animations.back()->elapsedTime = 0;
	animations.back()->currentLoops = 0;
	animations.back()->bools |= 0x8 * whole_animation_loop;
}
void MyAnimation::MakeMoveAnimConstSpeed(const std::vector<Instruction> instructions, MyVec3* _object, float speed, bool whole_animation_loop) {
	std::vector<Instruction> newI;
	for (const Instruction& i : instructions) {
		newI.push_back({ i.amount, length(i.amount) / speed, i.loops });
	}
	MakeMoveAnim(newI, _object, whole_animation_loop);
}
