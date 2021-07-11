#include "glad/glad.h"
#include "Loader.h"
#include "RawModel.h"

RawModel* Loader::loadToVao(float position[], int posSize, unsigned int indexData[], int indexSize)
{
	unsigned int vaoID = createVao(); // ����һ��VAO
	bindIndexBuffer(indexData, indexSize); // ��IBO
	storeDatatoAttriList(0, position, posSize); // ����VAO���浽�����б��0��λ����
	unbindVao(); // ���VAO
	RawModel* tmpModel = new RawModel(vaoID, indexSize / sizeof(int)); //  indexSize / sizeof(int)��ʾ�������
	models.push_back(tmpModel);
	return tmpModel;
}

// Denlete ���ɵ�VAO��VBO��ÿ��glGenVertexArrays���ɵĶ���Ҫ��Ӧ��delete
void Loader::cleanUp()
{
	if (models.size() > 0) {
		for(RawModel * item : models) {
			delete item;
		}
	}
	if (vaos.size() > 0) {
		glDeleteVertexArrays(vaos.size(), vaos.data());
	}
	if (vbos.size() > 0) {
		glDeleteBuffers(vbos.size(), vbos.data());
	}
}

unsigned int Loader::createVao()
{
	unsigned int vaoID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID); // �󶨴�VAO��״̬������������ 
	vaos.push_back(vaoID);
	return vaoID;
}

void Loader::unbindVao()
{
	glBindVertexArray(0); // ���VAO
}

void Loader::bindIndexBuffer(unsigned int indexBuffer[], int size)
{
	unsigned int iboID;
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID); // �󶨱�����IBO����
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indexBuffer, GL_STATIC_DRAW);
	// ���ܵ���glBindBuffer(GL_ARRAY_BUFFER, 0); �� ����VAO��ʱ����Զ�����ֶ��������
}

// ��Vbo�󶨵�VAO�������б��idxλ����
void Loader::storeDatatoAttriList(int idx, float positions[], int size)
{
	// create a VBO
	unsigned int vboID;
	glGenBuffers(1, &vboID); // ʹ��glGenBuffers������һ������ID����һ��VBO���󣬸ö���ʹ��vboID����ָ����
	glBindBuffer(GL_ARRAY_BUFFER, vboID); // ��VBO���˺�ʹ�õ��κ���GL_ARRAY_BUFFERĿ���ϵĻ�����ö����������õ�ǰ�󶨵Ļ���(VBO)
	glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW); // ��󶨵�VBO����������
	/*	
		glBufferData�Ĳ������ͣ�
		���ĵ�һ��������Ŀ�껺������ͣ����㻺�����ǰ�󶨵�GL_ARRAY_BUFFERĿ���ϡ�
		�ڶ�������ָ���������ݵĴ�С(���ֽ�Ϊ��λ)����һ���򵥵�sizeof������������ݴ�С���С�
		����������������ϣ�����͵�ʵ�����ݡ�
		���ĸ�����ָ��������ϣ���Կ���ι�����������ݡ�����������ʽ��
			GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
			GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
			GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
	*/
	glVertexAttribPointer(idx, 3, GL_FLOAT, false, 3 * sizeof(float), 0);// ���ݷ��͵�VBO�Ժ󣬽�VBOָ������Ӧ�������б��λ����
	glBindBuffer(GL_ARRAY_BUFFER, 0); // ���VBO
	vbos.push_back(vboID);
}
