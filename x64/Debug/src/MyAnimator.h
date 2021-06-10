#pragma once

#include "Model/Model.h"
#include "MyAnimation.h"

class MyAnimator
{
public:
	MyAnimator();
	MyAnimator(Model* model, MyAnimation* animation);
	void update();
	void increaseTime();
	void calculateCurrentPose();
	Model* model;
	MyAnimation* animation;
	float animTime;
	float lastTime;
	bool debug;
};

