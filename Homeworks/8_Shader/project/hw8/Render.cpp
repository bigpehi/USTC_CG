#include "Render.h"
#include "RawModel.h"
#include "glad/glad.h"

Render::Render()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); // 仅绘制框线
}

void Render::prepare()
{
	glClearColor(0.2, 0.3, 0.3, 1.0); // 设置清屏使用的颜色（状态设置函数）
	glClear(GL_COLOR_BUFFER_BIT); // 清空屏幕的COLOR BUFFER（状态使用函数）
}

void Render::onRender(RawModel*& model)
{
	if (!model) return;

	glBindVertexArray(model->getVaoID()); // 绑定VAO
	glEnableVertexAttribArray(0); // 启用顶点属性，顶点属性默认是禁用的
	//glDrawArrays(GL_TRIANGLES, 0, model->getVertexCnt()); // 使用当前激活的着色器，之前定义的顶点属性配置，和VBO的顶点数据（通过VAO间接绑定）来绘制图元
	glDrawElements(GL_TRIANGLES, model->getVertexCnt(), GL_UNSIGNED_INT, 0); // 使用当前激活的着色器，VAO进行绘制
	glDisableVertexAttribArray(0); // 绘制完成，disable属性列表0
	glBindVertexArray(0); // 绘制完成，解绑VAO
}
