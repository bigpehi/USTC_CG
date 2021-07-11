#pragma once
#include <vector>

// ����VAO��VBO��IBO
class Loader {
public:
	// position��Ϊһ������ָ�룬�޷�֪����Ԫ�ظ�������Ҫ��һ������size
	class RawModel* loadToVao(float position[], int posSize, unsigned int indexData[], int indexSize);
	void cleanUp();

private:
	unsigned int createVao(); // ����VAO
	void unbindVao(); // ���VAO

	void bindIndexBuffer(unsigned int indexBuffer[], int size); // ��IBO

	void storeDatatoAttriList(int idx, float positions[], int size); // �����ݴ浽�����б�(��16��)�Ķ�Ӧλ��

private:
	std::vector<class RawModel*> models;
	std::vector<unsigned int> vaos;
	std::vector<unsigned int> vbos;
};