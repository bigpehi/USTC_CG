#pragma once

// ���������ý�����������¼һ��VAO��ID��������Ķ������
class RawModel {
private:
	unsigned int vaoID; // �����������Vertex Array Object��VAO
	int vertexCnt;

public:
	RawModel() :vaoID(0), vertexCnt(0) {}

	RawModel(unsigned int _vaoID, int _vertexCnt) :vaoID(_vaoID), vertexCnt(_vertexCnt) {}

	unsigned int getVaoID() const { return vaoID; }
	int getVertexCnt() const { return vertexCnt; }
};