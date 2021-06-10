#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>

class MyJoint
{
public:
	MyJoint* parent;
	std::vector<MyJoint*> children;
	int index;
	std::string name;
	glm::mat4 transform;
	glm::mat4 inverseBindTransform;
};

