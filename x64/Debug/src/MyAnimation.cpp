#include "MyAnimation.h"

#include <assimp/Importer.hpp>
#include <assimp/types.h>
#include <assimp/scene.h>
#include <assimp\postprocess.h>
#include <iostream>

MyAnimation::MyAnimation(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    /* OLD ANIM LOADING CODE
    //Loop through every keyframe
    for (int i = 0; i < scene->mAnimations[0]->mChannels[0]->mNumPositionKeys; i++) {
        time = scene->mAnimations[0]->mDuration;
        //Loop through every bone in the current keyframe
        //Get time
        float timeOfKey = scene->mAnimations[0]->mChannels[0]->mPositionKeys[i].mTime;
        //Get all poses
        std::vector<MyJointTransform> trans;
        for (int j = 0; j < scene->mAnimations[0]->mNumChannels; j++) {
            std::cout << scene->mAnimations[0]->mChannels[j]->mNodeName.C_Str() << std::endl;
            MyJointTransform jt(glm::vec3(scene->mAnimations[0]->mChannels[j]->mPositionKeys[i].mValue.x, scene->mAnimations[0]->mChannels[j]->mPositionKeys[i].mValue.y, scene->mAnimations[0]->mChannels[j]->mPositionKeys[i].mValue.z), new MyQuaternion(scene->mAnimations[0]->mChannels[j]->mRotationKeys[i].mValue.x, scene->mAnimations[0]->mChannels[j]->mRotationKeys[i].mValue.y, scene->mAnimations[0]->mChannels[j]->mRotationKeys[i].mValue.z, scene->mAnimations[0]->mChannels[j]->mRotationKeys[i].mValue.w));
            trans.push_back(jt);
        }
        MyKeyframe* key = new MyKeyframe();
        key->jointTransforms = trans;
        key->timestamp = timeOfKey;
        keyframes.push_back(*key);
    }*/

    //NEW ANIM LOADING CODE
    this->time = scene->mAnimations[0]->mDuration;
    for (int i = 0; i < scene->mAnimations[0]->mChannels[0]->mNumPositionKeys; i++) {
        MyKeyframe* keyframe = new MyKeyframe();
        for (int j = 0; j < scene->mAnimations[0]->mNumChannels; j++) {
            //std::cout << "frame " << i << " bone" << j << std::endl;
            MyJointTransform* jt = new MyJointTransform(glm::vec3(scene->mAnimations[0]->mChannels[j]->mPositionKeys[i].mValue.x, scene->mAnimations[0]->mChannels[j]->mPositionKeys[i].mValue.y, scene->mAnimations[0]->mChannels[j]->mPositionKeys[i].mValue.z), scene->mAnimations[0]->mChannels[j]->mRotationKeys[i].mValue);
            float animTimeStamp = (float)scene->mAnimations[0]->mChannels[0]->mPositionKeys[i].mTime;
            keyframe->jointTransforms.push_back(jt);
            keyframe->timestamp = animTimeStamp;
        }
        this->keyframes.push_back(keyframe);

    }

    //std::cout << "Finished loading animation keyframes" << std::endl;
}