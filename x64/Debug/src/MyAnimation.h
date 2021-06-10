#pragma once
#include <assimp/anim.h>
#include "MyKeyframe.h"
#include <vector>

class MyAnimation
{
public:
	MyAnimation(std::string path);
	std::vector<MyKeyframe*> keyframes; //KeyFrame[] -> JointTransform[] joints(pos, rot), float time
	float time;
};

