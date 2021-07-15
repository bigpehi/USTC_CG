#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include "Display.h"
#include "Cube.h"

#include <iostream>

int main()
{
    Display mDisplay;
    mDisplay.create(); // 初始化glfw，设置版本号，核心模式，设置指定大小的窗口对象并绑定，加载glad，注册关于window和view的回调函数
  
    stbi_set_flip_vertically_on_load(false);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader modelShader("../Shader/model_loading.vs", "../Shader/model_loading.fs");
    Shader lightShader("../Shader/light_cube_loading.vs", "../Shader/light_cube_loading.fs");
    Shader floorShader("../Shader/light_cube_loading.vs", "../Shader/light_cube_loading.fs");

    // load models
    //Model ourModel("../data/model/david/David328.obj");
    Model ourModel("../data/model/nanosuit/nanosuit.obj");
    Cube lightCube(0., 18., 5., 1., 1., 1.);
    Cube floorCube(0.,-5.,0.,30.,1.,30.);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(mDisplay.mWindow))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        mDisplay.deltaTime = currentFrame - mDisplay.lastFrame;
        mDisplay.lastFrame = currentFrame;

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(mDisplay.camera.Zoom), (float)mDisplay.get_width() / (float)mDisplay.get_height(), 0.1f, 100.0f);
        glm::mat4 view = mDisplay.camera.GetViewMatrix();

        // configuration of floor shader
        floorShader.use();
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("view", view);
        glm::mat4 floor = glm::mat4(1.0f);
        floorShader.setMat4("model", floor);
        floorShader.setVec3("lightColor", glm::vec3(0.2)); // 设置方块颜色
        floorCube.Draw(floorShader);

        // configuration of light shader
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        glm::mat4 light = glm::mat4(1.0f);
        light = glm::rotate(light, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0., 1, 0.));
        lightShader.setMat4("model", light);
        lightShader.setVec3("lightColor", glm::vec3(1.0)); // 设置方块颜色
        lightCube.Draw(lightShader);

        // 计算光源位置
        glm::vec4 lightPos = glm::vec4(lightCube.getPosition(), 1.f);
        lightPos = light * lightPos;

        // configuration of model shader
        modelShader.use();
        modelShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("lightPos", lightPos[0], lightPos[1], lightPos[2]);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);
        ourModel.Draw(modelShader);

        mDisplay.update(); // 检查事件，交换颜色缓冲，处理按键
    }

    mDisplay.destroy(); // "glfwDestroyWindow" destroy the window and context, "glfwTerminate" release resource
    return 0;
}

