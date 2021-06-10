#include "MyAnimator.h"
#include <glfw/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

MyAnimator::MyAnimator() { }

MyAnimator::MyAnimator(Model* inModel, MyAnimation* inAnimation) {
	model = inModel;
	animation = inAnimation;
	animTime = 0;
	lastTime = 0;
	debug = true;
}

void applyPoseToJointAndChildren(MyJoint* joint, glm::vec3 pos, glm::quat quat) {
	joint->transform = glm::translate(joint->transform, pos);
	glm::mat4 rotmat = glm::toMat4(quat);
	joint->transform *= rotmat;
	for (int i = 0; i < joint->children.size(); i++) {
		applyPoseToJointAndChildren(joint->children[i], pos, quat);
	}
}

void MyAnimator::update() {
	if (animation == NULL) {
		std::cout << "Animator has no animation to update" << std::endl;
		return;
	}
	increaseTime();
	calculateCurrentPose();
	//calc pose for model
	if (debug) { //Debug code to set initial pose
		for (int i = 0; i < animation->keyframes[0]->jointTransforms.size(); i++) {
			applyPoseToJointAndChildren(model->jointIndexVector[i], glm::vec3(animation->keyframes[0]->jointTransforms[i]->pos.x, animation->keyframes[0]->jointTransforms[i]->pos.y, animation->keyframes[0]->jointTransforms[i]->pos.z), glm::quat(animation->keyframes[0]->jointTransforms[i]->rot.w, animation->keyframes[0]->jointTransforms[i]->rot.x, animation->keyframes[0]->jointTransforms[i]->rot.y, animation->keyframes[0]->jointTransforms[i]->rot.z));
		}
		debug = false;
	}
	//apply pose
	
}

void MyAnimator::increaseTime() {
	//Update time based on last run
	if (lastTime == 0) { //First run
		lastTime = glfwGetTime();
	}
	else { //Not first run
		float currTime = glfwGetTime();
		float deltaTime = currTime - lastTime;
		lastTime = currTime;
		animTime += deltaTime;
	}
	//Reset anim if time is finished
	if (animTime > animation->time) {
		animTime = 0;
	}
	//Log current time
	//std::cout << "TIME " << animTime << std::endl;
}

void MyAnimator::calculateCurrentPose() {
	//Clear current pose
	for (int i = 0; i < model->jointIndexVector.size(); i++) {
		model->jointIndexVector[i]->transform = glm::mat4(1.0f);
	}
	//Gets the previous and next frame to interpolate, TODO: can be optimized
	MyKeyframe* frame1 = NULL;
	MyKeyframe* frame2 = NULL;
	for (int i = 0; i < animation->keyframes.size(); i++) {
		if (animTime > animation->keyframes[i]->timestamp) {
			frame1 = animation->keyframes[i];
			frame2 = animation->keyframes[i + 1];
		} else if (animTime == 0) {
			frame1 = animation->keyframes[0];
			frame2 = animation->keyframes[1];
		}
	}
	//Calculate progression between both keyframes
	float timeBetween = frame2->timestamp - frame1->timestamp;  //Seconds between frame 1 and frame 2
	float timeProgression = animTime - frame1->timestamp;		//Progression through the frame in seconds
	float progression = timeProgression / timeBetween;			//Get a progression value between 0 and 1

	//Interpolate values
	//interpolate(frame1, frame2, progression), MyJoint interpolatedTransforms[joints.size()]
	std::vector<MyJointTransform> interpolatedTransforms;
	interpolatedTransforms.resize(model->jointIndexVector.size());

	//Interpolate each joint
	for (int i = 0; i < interpolatedTransforms.size(); i++) {
		//Interpolate values
		glm::vec3 finalPos = glm::vec3();
		finalPos = glm::mix(frame1->jointTransforms[i]->pos, frame2->jointTransforms[i]->pos, progression);
		aiQuaternion finalRot;
		aiQuaternion::Interpolate(finalRot, frame1->jointTransforms[i]->rot, frame2->jointTransforms[i]->rot, progression);

		interpolatedTransforms[i].pos = finalPos;
		interpolatedTransforms[i].rot = finalRot;
	}

	for (int i = 0; i < animation->keyframes[0]->jointTransforms.size(); i++) {
		applyPoseToJointAndChildren(model->jointIndexVector[i], interpolatedTransforms[i].pos, glm::quat(interpolatedTransforms[i].rot.w, interpolatedTransforms[i].rot.x, interpolatedTransforms[i].rot.y, interpolatedTransforms[i].rot.z));
	}

}