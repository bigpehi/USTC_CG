#pragma once

// 这个类的作用仅仅是用来记录一个VAO的ID和其关联的顶点个数
class RawModel {
private:
	unsigned int vaoID; // 顶点数组对象：Vertex Array Object，VAO
	int vertexCnt;

public:
	RawModel() :vaoID(0), vertexCnt(0) {}

	RawModel(unsigned int _vaoID, int _vertexCnt) :vaoID(_vaoID), vertexCnt(_vertexCnt) {}

	unsigned int getVaoID() const { return vaoID; }
	int getVertexCnt() const { return vertexCnt; }
};