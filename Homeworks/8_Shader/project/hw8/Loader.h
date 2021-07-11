#pragma once
#include <vector>

// 管理VAO、VBO、IBO
class Loader {
public:
	// position作为一个数组指针，无法知道其元素个数，需要多一个变量size
	class RawModel* loadToVao(float position[], int posSize, unsigned int indexData[], int indexSize);
	void cleanUp();

private:
	unsigned int createVao(); // 创建VAO
	void unbindVao(); // 解绑VAO

	void bindIndexBuffer(unsigned int indexBuffer[], int size); // 绑定IBO

	void storeDatatoAttriList(int idx, float positions[], int size); // 将数据存到属性列表(共16个)的对应位置

private:
	std::vector<class RawModel*> models;
	std::vector<unsigned int> vaos;
	std::vector<unsigned int> vbos;
};