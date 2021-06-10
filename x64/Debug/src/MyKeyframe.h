#pragma once

#include "MyJointTransform.h"
#include <vector>

class MyKeyframe
{
public:
	MyKeyframe();
	std::vector<MyJointTransform*> jointTransforms;
	float timestamp;
};

