#include "MyJointTransform.h"
#include <assimp/quaternion.h>

MyJointTransform::MyJointTransform() { }

MyJointTransform::MyJointTransform(glm::vec3 inPos, aiQuaternion inRot) {
	pos = inPos;
	rot = inRot;
}