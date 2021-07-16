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

void RenderQuad();

int main()
{
    Display mDisplay;
    mDisplay.create(); // 初始化glfw，设置版本号，核心模式，设置指定大小的窗口对象并绑定，加载glad，注册关于window和view的回调函数
  
    stbi_set_flip_vertically_on_load(false);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader modelShader("../Shader/model_loading.vs", "../Shader/model_loading.fs");
    Shader dShader("../Shader/shadow_mapping.vs", "../Shader/shadow_mapping.fs"); // 这里只针对地板
    Shader simpleDepthShader("../Shader/shadow_mapping_depth.vs", "../Shader/shadow_mapping_depth.fs");
    Shader lightShader("../Shader/light_cube_loading.vs", "../Shader/light_cube_loading.fs");
    Shader debugDepthQuad("../Shader/debug_quad.vs", "../Shader/debug_quad.fs"); // 显示深度图
    //Shader floorShader("../Shader/general_cube.vs", "../Shader/general_cube.fs");

    dShader.use();
    glUniform1i(glGetUniformLocation(dShader.ID, "shadowMap"), 0);

    // load models
    //Model ourModel("../data/model/david/David328.obj");
    Model ourModel("../data/model/nanosuit/nanosuit.obj");
    Cube lightCube(0., 20., 5., .3, .3, .3);
    Cube floorCube(0.,-1.,0.,50.,1.,50.);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    //sss////////////////////////////////////////////////////////////////////////
    // Configure depth map FBO
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO); // 帧缓冲对象
    // - Create depth texture, 深度信息都保存在这张深度纹理中
    GLuint depthMap;
    glGenTextures(1, &depthMap); // 纹理对象
    glBindTexture(GL_TEXTURE_2D, depthMap); // 绑定纹理对象，后面都是对他操作
    // 纹理相关设置
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // 纹理过滤方式，采用邻近过滤，会有锯齿的那种，后面可以改成GL_LINEAR试试
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // 纹理环绕方式，超出的坐标为用户指定的边缘颜色。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // 深度纹理未覆盖到的地方不产生阴影，深度均设为1.0，为最大深度
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // 把我们生成的深度纹理作为帧缓冲的深度缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); // 绑定帧缓冲，后面的操作都针对他
    // 将纹理附加到帧缓冲上
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    /*
        glFrameBufferTexture2D有以下的参数：
        target：帧缓冲的目标（绘制、读取或者两者皆有）
        attachment：我们想要附加的附件类型。当前我们正在附加一个颜色附件。注意最后的0意味着我们可以附加多个颜色附件。我们将在之后的教程中提到。
        textarget：你希望附加的纹理类型
        texture：要附加的纹理本身
        level：多级渐远纹理的级别。我们将它保留为0。
    */
    glDrawBuffer(GL_NONE); // 显式告诉OpenGL我们不使用任何颜色数据进行渲染
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解绑深度缓冲，交还给默认帧缓冲
    //eee////////////////////////////////////////////////////////////////////////

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);


    // render loop
    while (!glfwWindowShouldClose(mDisplay.mWindow))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        mDisplay.deltaTime = currentFrame - mDisplay.lastFrame;
        mDisplay.lastFrame = currentFrame;

        // 计算光源位置
        glm::mat4 light = glm::mat4(1.0f);
        light = glm::rotate(light, (float)glfwGetTime() * glm::radians(80.0f), glm::vec3(0., 1., 0.));
        glm::vec4 lightPos = glm::vec4(lightCube.getPosition(), 1.f);
        lightPos = light * lightPos;
        //glm::vec3 lightPosV3 = glm::vec3(lightPos[0], lightPos[1], lightPos[2]);
        glm::vec3 lightPosV3(lightPos[0], lightPos[1], lightPos[2]);

        //sss///////////////////////////////////////////////////////////////////////////
        // 1. Render depth of scene to texture (from light's perspective)
        // - Get light projection/view matrix.
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        GLfloat near_plane = 1.0f, far_plane = 57.5f;
        //lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightProjection = glm::perspective(80.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
        //lightProjection = glm::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to refl
        lightView = glm::lookAt(lightPosV3, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // - now render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        //glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        //RenderScene(simpleDepthShader);

        glm::mat4 floor = glm::mat4(1.0f);
        simpleDepthShader.setMat4("model", floor);
        floorCube.Draw();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        simpleDepthShader.setMat4("model", model);
        ourModel.Draw(simpleDepthShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //eee///////////////////////////////////////////////////////////////////////////

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(mDisplay.camera.Zoom), (float)mDisplay.get_width() / (float)mDisplay.get_height(), 0.1f, 100.0f);
        glm::mat4 view = mDisplay.camera.GetViewMatrix();

        // configuration of light shader
        glViewport(0, 0, 800, 600);
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        lightShader.setMat4("model", light);
        lightShader.setVec3("lightColor", glm::vec3(1.0)); // 设置方块颜色
        lightCube.Draw();

        //////////////////////////////////////////////////////////////////////
        // Set light uniforms
        dShader.use(); // 用shader之前一定要use啊，调了一天T_T
        dShader.setVec3("lightPos", lightPosV3);
        dShader.setVec3("viewPos", mDisplay.camera.GetPosition());

        dShader.setMat4("lightSpaceMatrix1", lightSpaceMatrix);
        //////////////////////////////////////////////////////////////////////

        // configuration of floor shader
        
        dShader.setMat4("projection", projection);
        dShader.setMat4("view", view);
        floor = glm::mat4(1.0f);
        dShader.setMat4("model", floor);
        dShader.setVec3("lightColor", glm::vec3(1.0)); // 设置光源颜色
        dShader.setVec3("cubeColor", glm::vec3(0.2)); // 设置物体颜色
        /////////////////////////////////////////////////
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        ///////////////////////////////////////////////////////
        floorCube.Draw();

        // configuration of model shader
        modelShader.use();
        modelShader.setVec3("lightColor", glm::vec3(1.0));
        modelShader.setVec3("lightPos", lightPos[0], lightPos[1], lightPos[2]);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);
        ourModel.Draw(modelShader);

        ///////////////////////////////////////////////////////////////
        // 3. DEBUG: visualize depth map by rendering it to plane
        //debugDepthQuad.use();
        //glUniform1f(glGetUniformLocation(debugDepthQuad.ID, "near_plane"), near_plane);
        //glUniform1f(glGetUniformLocation(debugDepthQuad.ID, "far_plane"), far_plane);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, depthMap);
        //RenderQuad();
        ///////////////////////////////////////////////////////////////


        mDisplay.update(); // 检查事件，交换颜色缓冲，处理按键
    }

    mDisplay.destroy(); // "glfwDestroyWindow" destroy the window and context, "glfwTerminate" release resource
    return 0;
}

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}