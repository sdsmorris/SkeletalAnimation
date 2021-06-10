#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader/Shader.h"
#include "Camera/Camera.h"
#include "Model/Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include "MyAnimator.h"
#include "MyAnimation.h"
#include "MyJoint.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void sendJointHierarchyToShader(Model* model, Shader* shader) {
    //std::cout << "set joint " << rootNode->index << rootNode->name << std::endl;
    /*if (rootNode->index == 14) {
        rootNode->transform = glm::translate(rootNode->transform, glm::vec3(0.001, 0.001, 0.001));
    }*/
    //TODO: don't need this anymore just use jointIndexList
    /*char name[16];
    memset(name, 0, sizeof(name));
    snprintf(name, sizeof(name), "bones[%d]", rootNode->index);
    shader->setMat4(name, rootNode->transform);
    for (int i = 0; i < rootNode->children.size(); i++) {
        sendJointHierarchyToShader(rootNode->children[i], shader);
    }*/
    char name[16];
    for (int i = 0; i < model->jointIndexVector.size(); i++) {
        memset(name, 0, sizeof(name));
        snprintf(name, sizeof(name), "bones[%d]", i);
        //concat = parents transform * current
        //globInverse * concat * inverse bind
        //model->globalInverseTransform * 
        glm::mat4 finalTrans = model->globalInverseTransform * model->jointIndexVector[i]->transform * model->jointIndexVector[i]->inverseBindTransform;
        shader->setMat4(name, finalTrans);
    }
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("src/Animation/vertex.glsl", "src/Animation/fragment.glsl");

    // load models
    // -----------
    //Model ourModel("C:/Users/sammo/Documents/Resources/Models/TestStuff/BackPack/backpack.obj");
    //Model ourModel("C:/Users/sammo/Documents/Resources/Models/TestStuff/Mario.dae");
    //Model ourModel("C:/Users/sammo/Documents/Resources/Models/Rips/Super Mario 64 DS/Wario/wario.obj");
    //Model ourModel("C:/Users/sammo/Documents/Blender/My Models/JavaPaint.dae");
    //Model ourModel("C:/Users/sammo/Documents/Blender/My Models/ThwompSM64.dae");
    Model ourModel("C:/Users/sammo/Documents/Resources/Tutorials/SkeletalAnimationJava/model.dae");
    //Model ourModel("C:/Users/sammo/Downloads/ogldev-source/ogldev-source/Content/boblampclean.md5mesh");
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //Load each channels key positions and rotations into a joint transform, then store each keys transforms in a keyframe with the keys time, and then put all the keys in an animation
    MyAnimation* animation = new MyAnimation("C:/Users/sammo/Documents/Resources/Tutorials/SkeletalAnimationJava/model.dae");
    //MyAnimation* animation = new MyAnimation("C:/Users/sammo/Downloads/ogldev-source/ogldev-source/Content/boblampclean.md5mesh");

    //Loop through all vertices and print their joint ids and weights
    /*for (int i = 0; i < ourModel.meshes[0].vertices.size(); i++) {
        std::cout << "VERTEX " << i << std::endl;
        float sum = 0;
        for (int j = 0; j < ourModel.meshes[0].vertices[i].allJointIDs.size(); j++) {
            std::cout << ourModel.meshes[0].vertices[i].allJointIDs[j] << ourModel.meshes[0].vertices[i].allJointWeights[j] << std::endl;
            sum += ourModel.meshes[0].vertices[i].allJointWeights[j];
        }
        std::cout << "SUM " << sum << std::endl;
    }*/

    for (int i = 0; i < ourModel.meshes[0].vertices.size(); i++) {
        //Order array so we can easily get top 3 most influential weights
        if (ourModel.meshes[0].vertices[i].allJointIDs.size() > 3) {
            for (int j = 0; j < ourModel.meshes[0].vertices[i].allJointWeights.size(); j++) {
                //Rearrange vectors

                //Shitty non-algorthim that doesn't grab 3 most influential weights just first 3
                for (int k = 0; k < 3; k++) {
                    ourModel.meshes[0].vertices[i].JointIDs[k] = ourModel.meshes[0].vertices[i].allJointIDs[k];
                    ourModel.meshes[0].vertices[i].Weights[k] = ourModel.meshes[0].vertices[i].allJointWeights[k];
                }

                //Print vectors
                //std::cout << "VERT:"<< i << " ";
                //std::cout << ourModel.meshes[0].vertices[i].allJointIDs[j] << " ";
                //std::cout << ourModel.meshes[0].vertices[i].allJointWeights[j] << " ";
            }
            //Normalize the 3 values to all equal 1
            float total = 0;
            for (int k = 0; k < 3; k++) {
                total += ourModel.meshes[0].vertices[i].Weights[k];
            }
            for (int k = 0; k < 3; k++) {
                ourModel.meshes[0].vertices[i].Weights[k] /= total;
            }

            /*std::cout << i << " " << "NORMALIZED ";
            for (int k = 0; k < 3; k++) {
                std::cout << ourModel.meshes[0].vertices[i].Weights[k] << " ";
            }
            std::cout << std::endl;*/
        } else if (ourModel.meshes[0].vertices[i].allJointIDs.size() == 2) {
            ourModel.meshes[0].vertices[i].JointIDs[0] = ourModel.meshes[0].vertices[i].allJointIDs[0];
            ourModel.meshes[0].vertices[i].Weights[0] = ourModel.meshes[0].vertices[i].allJointWeights[0];
            ourModel.meshes[0].vertices[i].JointIDs[1] = ourModel.meshes[0].vertices[i].allJointIDs[1];
            ourModel.meshes[0].vertices[i].Weights[1] = ourModel.meshes[0].vertices[i].allJointWeights[1];

            ourModel.meshes[0].vertices[i].JointIDs[2] = 0;
            ourModel.meshes[0].vertices[i].Weights[2] = 0.0f;

        } else if (ourModel.meshes[0].vertices[i].allJointIDs.size() == 1) {
            ourModel.meshes[0].vertices[i].JointIDs[0] = ourModel.meshes[0].vertices[i].allJointIDs[0];
            ourModel.meshes[0].vertices[i].Weights[0] = ourModel.meshes[0].vertices[i].allJointWeights[0];
            ourModel.meshes[0].vertices[i].JointIDs[1] = 0;
            ourModel.meshes[0].vertices[i].Weights[1] = 0.0f;
            ourModel.meshes[0].vertices[i].JointIDs[2] = 0;
            ourModel.meshes[0].vertices[i].Weights[2] = 0.0f;

        } else if (ourModel.meshes[0].vertices[i].allJointIDs.size() == 3) {
            for (int k = 0; k < 3; k++) {
                ourModel.meshes[0].vertices[i].JointIDs[k] = ourModel.meshes[0].vertices[i].allJointIDs[k];
                ourModel.meshes[0].vertices[i].Weights[k] = ourModel.meshes[0].vertices[i].allJointWeights[k];
            }
        }
    }

    ourModel.meshes[0].setupMesh();

    MyAnimator* animator = new MyAnimator(&ourModel, animation);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime(); //glfwGetTime() returns the current time in seconds since glfw  (or in this case the program) started
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //cout << deltaTime << endl;

        // input
        processInput(window);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //Render bone model
        /*
        //Get current time to get pose from animation
        float RunningTime = (float)((double)GetCurrentTimeMillis() - (double)m_startTime) / 1000.0f;

        //Update bone transforms, interpolate
        m_mesh.BoneTransform(RunningTime, Transforms);

        //Setting bone transforms in vertex shader
        for (uint i = 0; i < Transforms.size(); i++) {
            m_pEffect->SetBoneTransform(i, Transforms[i]);
        }
        */

        //Recursively send each bone to the shader

        //MyAnim stuff
        //model.animator.update()
        //for shader.setmat4(model.bones[i]);

        //Update bones
        animator->update();

        //Set bones
        sendJointHierarchyToShader(&ourModel, &ourShader);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);

        //This is a simple example of setting a uniform in an array, simply just add an index to the string name
        /*glm::mat4 test = glm::mat4(1.0f); //Set it from shader class
        test = glm::translate(test, glm::vec3(0, 0, -1.0f));
        ourShader.setMat4("bones[0]", test);/*
        /*glm::mat4 test = glm::mat4(1.0f); //Set it without shader class
        test = glm::translate(test, glm::vec3(0.0f, 0.0f, -1.0f));
        int testLoc = glGetUniformLocation(ourShader.ID, "bones[0]");
        glUniformMatrix4fv(testLoc, 1, GL_FALSE, glm::value_ptr(test));*/
        /*glm::mat4 test = glm::mat4(1.0f); //Set all values in array
        test = glm::translate(test, glm::vec3(0, 0, -1.0f));
        for (int i = 0; i < 50; i++) {
            char name[128];
            memset(name, 0, sizeof(name));
            snprintf(name, sizeof(name), "bones[%d]", i);
            ourShader.setMat4(name, test);
        }*/

        ourModel.Draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete animation;
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}