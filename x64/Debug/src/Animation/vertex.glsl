#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec3 boneIDs;
layout (location = 6) in vec3 weights;

out vec2 TexCoords;

const int MAX_BONES = 50;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bones[MAX_BONES];

void main()
{
    mat4 BoneTransform = bones[boneIDs[0]] * weights[0];
    BoneTransform     += bones[boneIDs[1]] * weights[1];
    BoneTransform     += bones[boneIDs[2]] * weights[2];
    //BoneTransform     += bones[boneIDs[3]] * weights[3]; //if using fourth bone id

    vec4 PosL    = BoneTransform * vec4(aPos, 1.0);

    gl_Position  = projection * view * model * PosL; //PosL, vec4(aPos, 1.0)
    
    TexCoords = aTexCoords;
}