#pragma once

#include <glm/glm.hpp>
#include <assimp/quaternion.h>

class MyJointTransform
{
public:
	MyJointTransform();
	MyJointTransform(glm::vec3 inPos, aiQuaternion inRot);
	glm::vec3 pos;
	aiQuaternion rot;
};

