#include "DisplayManager.h"
#include "Loader.h"
#include "RawModel.h"
#include "Render.h"
#include "StaticShader.h"
#include "MathTools.h"
#include <GLFW\glfw3.h>

int main()
{
    DisplayManager mDisplayMgr;
    mDisplayMgr.createDisplay();

    Loader mLoader;

    // --------------立方体顶点数据--------------
    //   0/----------------/1
    //   /|               /|
    //  / |              / |
    //4/--|-------------/5 |
    // |  |             |  |
    // |  |             |  |
    // |  |             |  |
    // | 3/-------------|--/2
    // | /              | /
    // |/               |/
    //7/----------------/6

    float positions[]{
        -0.5f,      0.5f,       -0.5f,
        0.5f,       0.5f,       -0.5f,
        0.5f,       -0.5f,      -0.5f,
        -0.5f,      -0.5f,      -0.5f,
        -0.5f,      0.5f,       0.5f,
        0.5f,       0.5f,       0.5f,
        0.5f,       -0.5f,      0.5f,
        -0.5f,      -0.5f,      0.5f,
    };

    unsigned int indexData[]{
        1, 5, 0, 0, 5, 4,
        0, 4, 3, 3, 4, 7,
        3, 2, 6, 3, 6, 7,
        5, 2, 6, 5, 1, 2,
        4, 5, 6, 4, 6, 7,
        0, 1, 3, 1, 2, 3
    };


    /*
    * 绑定VAO，根据positions生成VBO并送到缓冲中
    * 根据indextData生成EBO并绑定
    * 最后解绑VAO，mModel是一个自定义类，其中有两个属性：VAO的ID和涉及到的顶点数
    */
    RawModel* mModel = mLoader.loadToVao(positions, sizeof(positions), indexData, sizeof(indexData));

    /*
    * Render类用于控制渲染相关的设置，包括
    * 设置清屏颜色，绘制框线，绑定合适的VAO
    */
    Render mRender;

    /*
    * 利用staticshader中定义的shader文件路径编译链接着色器
    */
    StaticShader mShader;

    while (!mDisplayMgr.isRequestClosed())
    {
        mRender.prepare(); // 清屏

        mShader.start(); // 激活shader程序对象；reloadShader之后，这里会更新类属性shaderID，达到HotReload的效果
        mShader.loadTransformMatrix(GLMath::createTransformMatrix(GLMath::Vector3(), (float)glfwGetTime() * 50, (float)glfwGetTime() * 10, (float)glfwGetTime() * 50, 1)); // 发送一个矩阵到着色器
        mRender.onRender(mModel); // 绑定VAO，绘制，解绑
        mShader.stop(); // 解绑shader

        mDisplayMgr.updateDisplay(); // 检查事件，交换颜色缓冲、捕捉ESC
        if (mDisplayMgr.isKeyPressed(GLFW_KEY_R)) {
            mShader.reloadShader(); // 触发R键则编译链接新的shader
        }
    }

    mShader.cleanUp();
    mLoader.cleanUp();
    mDisplayMgr.closeDisplay();
    return 0;
}