#include "glad/glad.h"
#include "Loader.h"
#include "RawModel.h"

RawModel* Loader::loadToVao(float position[], int posSize, unsigned int indexData[], int indexSize)
{
	unsigned int vaoID = createVao(); // 创建一个VAO
	bindIndexBuffer(indexData, indexSize); // 绑定IBO
	storeDatatoAttriList(0, position, posSize); // 将此VAO保存到属性列表的0号位置上
	unbindVao(); // 解绑VAO
	RawModel* tmpModel = new RawModel(vaoID, indexSize / sizeof(int)); //  indexSize / sizeof(int)表示顶点个数
	models.push_back(tmpModel);
	return tmpModel;
}

// Denlete 生成的VAO和VBO，每个glGenVertexArrays生成的都需要相应的delete
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
	glBindVertexArray(vaoID); // 绑定此VAO到状态机的上下文中 
	vaos.push_back(vaoID);
	return vaoID;
}

void Loader::unbindVao()
{
	glBindVertexArray(0); // 解绑VAO
}

void Loader::bindIndexBuffer(unsigned int indexBuffer[], int size)
{
	unsigned int iboID;
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID); // 绑定变量到IBO对象
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indexBuffer, GL_STATIC_DRAW);
	// 不能调用glBindBuffer(GL_ARRAY_BUFFER, 0); ， 不用VAO的时候会自动解绑，手动解绑会出错
}

// 将Vbo绑定到VAO的属性列表的idx位置上
void Loader::storeDatatoAttriList(int idx, float positions[], int size)
{
	// create a VBO
	unsigned int vboID;
	glGenBuffers(1, &vboID); // 使用glGenBuffers函数和一个缓冲ID生成一个VBO对象，该对象使用vboID进行指定。
	glBindBuffer(GL_ARRAY_BUFFER, vboID); // 绑定VBO，此后使用的任何在GL_ARRAY_BUFFER目标上的缓冲调用都会用来配置当前绑定的缓冲(VBO)
	glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW); // 向绑定的VBO对象发送数据
	/*	
		glBufferData的参数解释：
		它的第一个参数是目标缓冲的类型：顶点缓冲对象当前绑定到GL_ARRAY_BUFFER目标上。
		第二个参数指定传输数据的大小(以字节为单位)；用一个简单的sizeof计算出顶点数据大小就行。
		第三个参数是我们希望发送的实际数据。
		第四个参数指定了我们希望显卡如何管理给定的数据。它有三种形式：
			GL_STATIC_DRAW ：数据不会或几乎不会改变。
			GL_DYNAMIC_DRAW：数据会被改变很多。
			GL_STREAM_DRAW ：数据每次绘制时都会改变。
	*/
	glVertexAttribPointer(idx, 3, GL_FLOAT, false, 3 * sizeof(float), 0);// 数据发送到VBO以后，将VBO指定到相应的属性列表的位置上
	glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑VBO
	vbos.push_back(vboID);
}
