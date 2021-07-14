#include "Render.h"
#include "RawModel.h"
#include "glad/glad.h"

Render::Render()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); // �����ƿ���
}

void Render::prepare()
{
	glClearColor(0.2, 0.3, 0.3, 1.0); // ��������ʹ�õ���ɫ��״̬���ú�����
	glClear(GL_COLOR_BUFFER_BIT); // �����Ļ��COLOR BUFFER��״̬ʹ�ú�����
}

void Render::onRender(RawModel*& model)
{
	if (!model) return;

	glBindVertexArray(model->getVaoID()); // ��VAO
	glEnableVertexAttribArray(0); // ���ö������ԣ���������Ĭ���ǽ��õ�
	//glDrawArrays(GL_TRIANGLES, 0, model->getVertexCnt()); // ʹ�õ�ǰ�������ɫ����֮ǰ����Ķ����������ã���VBO�Ķ������ݣ�ͨ��VAO��Ӱ󶨣�������ͼԪ
	glDrawElements(GL_TRIANGLES, model->getVertexCnt(), GL_UNSIGNED_INT, 0); // ʹ�õ�ǰ�������ɫ����VAO���л���
	glDisableVertexAttribArray(0); // ������ɣ�disable�����б�0
	glBindVertexArray(0); // ������ɣ����VAO
}
