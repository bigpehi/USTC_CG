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
    mDisplay.create(); // ��ʼ��glfw�����ð汾�ţ�����ģʽ������ָ����С�Ĵ��ڶ��󲢰󶨣�����glad��ע�����window��view�Ļص�����
  
    stbi_set_flip_vertically_on_load(false);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader modelShader("../Shader/model_loading.vs", "../Shader/model_loading.fs");
    Shader dShader("../Shader/shadow_mapping.vs", "../Shader/shadow_mapping.fs"); // ����ֻ��Եذ�
    Shader simpleDepthShader("../Shader/shadow_mapping_depth.vs", "../Shader/shadow_mapping_depth.fs");
    Shader lightShader("../Shader/light_cube_loading.vs", "../Shader/light_cube_loading.fs");
    Shader debugDepthQuad("../Shader/debug_quad.vs", "../Shader/debug_quad.fs"); // ��ʾ���ͼ
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
    glGenFramebuffers(1, &depthMapFBO); // ֡�������
    // - Create depth texture, �����Ϣ���������������������
    GLuint depthMap;
    glGenTextures(1, &depthMap); // �������
    glBindTexture(GL_TEXTURE_2D, depthMap); // ��������󣬺��涼�Ƕ�������
    // �����������
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // ������˷�ʽ�������ڽ����ˣ����о�ݵ����֣�������Ըĳ�GL_LINEAR����
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // �����Ʒ�ʽ������������Ϊ�û�ָ���ı�Ե��ɫ��
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // �������δ���ǵ��ĵط���������Ӱ����Ⱦ���Ϊ1.0��Ϊ������
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // ���������ɵ����������Ϊ֡�������Ȼ���
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); // ��֡���壬����Ĳ����������
    // �������ӵ�֡������
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    /*
        glFrameBufferTexture2D�����µĲ�����
        target��֡�����Ŀ�꣨���ơ���ȡ�������߽��У�
        attachment��������Ҫ���ӵĸ������͡���ǰ�������ڸ���һ����ɫ������ע������0��ζ�����ǿ��Ը��Ӷ����ɫ���������ǽ���֮��Ľ̳����ᵽ��
        textarget����ϣ�����ӵ���������
        texture��Ҫ���ӵ�������
        level���༶��Զ����ļ������ǽ�������Ϊ0��
    */
    glDrawBuffer(GL_NONE); // ��ʽ����OpenGL���ǲ�ʹ���κ���ɫ���ݽ�����Ⱦ
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // �����Ȼ��壬������Ĭ��֡����
    //eee////////////////////////////////////////////////////////////////////////

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);


    // render loop
    while (!glfwWindowShouldClose(mDisplay.mWindow))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        mDisplay.deltaTime = currentFrame - mDisplay.lastFrame;
        mDisplay.lastFrame = currentFrame;

        // �����Դλ��
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
        lightShader.setVec3("lightColor", glm::vec3(1.0)); // ���÷�����ɫ
        lightCube.Draw();

        //////////////////////////////////////////////////////////////////////
        // Set light uniforms
        dShader.use(); // ��shader֮ǰһ��Ҫuse��������һ��T_T
        dShader.setVec3("lightPos", lightPosV3);
        dShader.setVec3("viewPos", mDisplay.camera.GetPosition());

        dShader.setMat4("lightSpaceMatrix1", lightSpaceMatrix);
        //////////////////////////////////////////////////////////////////////

        // configuration of floor shader
        
        dShader.setMat4("projection", projection);
        dShader.setMat4("view", view);
        floor = glm::mat4(1.0f);
        dShader.setMat4("model", floor);
        dShader.setVec3("lightColor", glm::vec3(1.0)); // ���ù�Դ��ɫ
        dShader.setVec3("cubeColor", glm::vec3(0.2)); // ����������ɫ
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


        mDisplay.update(); // ����¼���������ɫ���壬������
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