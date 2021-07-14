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

    // --------------�����嶥������--------------
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
    * ��VAO������positions����VBO���͵�������
    * ����indextData����EBO����
    * �����VAO��mModel��һ���Զ����࣬�������������ԣ�VAO��ID���漰���Ķ�����
    */
    RawModel* mModel = mLoader.loadToVao(positions, sizeof(positions), indexData, sizeof(indexData));

    /*
    * Render�����ڿ�����Ⱦ��ص����ã�����
    * ����������ɫ�����ƿ��ߣ��󶨺��ʵ�VAO
    */
    Render mRender;

    /*
    * ����staticshader�ж����shader�ļ�·������������ɫ��
    */
    StaticShader mShader;

    while (!mDisplayMgr.isRequestClosed())
    {
        mRender.prepare(); // ����

        mShader.start(); // ����shader�������reloadShader֮����������������shaderID���ﵽHotReload��Ч��
        mShader.loadTransformMatrix(GLMath::createTransformMatrix(GLMath::Vector3(), (float)glfwGetTime() * 50, (float)glfwGetTime() * 10, (float)glfwGetTime() * 50, 1)); // ����һ��������ɫ��
        mRender.onRender(mModel); // ��VAO�����ƣ����
        mShader.stop(); // ���shader

        mDisplayMgr.updateDisplay(); // ����¼���������ɫ���塢��׽ESC
        if (mDisplayMgr.isKeyPressed(GLFW_KEY_R)) {
            mShader.reloadShader(); // ����R������������µ�shader
        }
    }

    mShader.cleanUp();
    mLoader.cleanUp();
    mDisplayMgr.closeDisplay();
    return 0;
}