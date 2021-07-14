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
    mDisplay.create(); // ��ʼ��glfw�����ð汾�ţ�����ģʽ������ָ����С�Ĵ��ڶ��󲢰󶨣�����glad��ע�����window��view�Ļص�����
  
    stbi_set_flip_vertically_on_load(false);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader modelShader("../Shader/model_loading.vs", "../Shader/model_loading.fs");
    Shader lightShader("../Shader/light_cube_loading.vs", "../Shader/light_cube_loading.fs");

    // load models
    //Model ourModel("../data/model/david/David328.obj");
    Model ourModel("../data/model/nanosuit/nanosuit.obj");
    Cube lightCube(0.,20.,0.,1.,1.,1.);

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

        // don't forget to enable shader before setting uniforms
        modelShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(mDisplay.camera.Zoom), (float)mDisplay.get_width() / (float)mDisplay.get_height(), 0.1f, 100.0f);
        glm::mat4 view = mDisplay.camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);




        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);
        ourModel.Draw(modelShader);

        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        glm::mat4 light = glm::mat4(1.0f);
        lightShader.setMat4("model", light);
        lightShader.setVec3("lightColor", glm::vec3(1.0)); // ���÷�����ɫ
        lightCube.Draw(lightShader);

        mDisplay.update(); // ����¼���������ɫ���壬������
    }

    mDisplay.destroy(); // "glfwDestroyWindow" destroy the window and context, "glfwTerminate" release resource
    return 0;
}

