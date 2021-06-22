#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

MinSurf::MinSurf(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void MinSurf::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool MinSurf::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure 对象初始化使用的是三角面片，这里将该三角面片转换为半边结构
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's positions ->  half-edge structure's positions 
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Minimize();

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos.cast_to<pointf3>());
	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}

	triMesh->Init(indice, positions);

	return true;
}

void MinSurf::Minimize() {
	// step1：构造稀疏的系数矩阵，行列数为mesh中的点数，根据邻接关系和重心计算方法填充矩阵中的非零元素
	size_t nV = heMesh->NumVertices(); // mesh的点数
	SparseMatrix<double> A_sparse(nV, nV);
	A_sparse.setZero();
	// step2：构造列向量
	VectorXd B_x(nV); B_x.setZero();
	VectorXd B_y(nV); B_y.setZero();
	VectorXd B_z(nV); B_z.setZero();
	VectorXd X_x(nV); X_x.setZero();
	VectorXd X_y(nV); X_y.setZero();
	VectorXd X_z(nV); X_z.setZero();
	// step3：设置系数矩阵与列向量中的非零项
	for (size_t i = 0; i < nV; i++) {
		A_sparse.insert(i, i) = 1; // 对角线都为1
		V* v_i = heMesh->Vertices().at(i); 
		if (!v_i->IsBoundary()) {
			// 设置系数矩阵A，重心求解直接用邻点的平均
			double v_i_degree = v_i->AdjVertices().size(); // 顶点v_i的度
			for (size_t j = 0; j < v_i_degree; j++) {
				size_t v_j_index = heMesh->Index(v_i->AdjVertices().at(j)); // 顶点v_i的第j个邻点的index
				A_sparse.insert(i, v_j_index) = -1 / v_i_degree;
				//A_sparse.insert(i, heMesh->Index(v_i->AdjVertices()[j]) ) = -1 / v_i_degree;
			}
		}	
		else{ // vetex v_i is boundary
			// 设置列向量B（为保证边界点不动，需要将B矩阵的对应列设为原值）
			B_x(i) = v_i->pos.at(0);
			B_y(i) = v_i->pos.at(1);
			B_z(i) = v_i->pos.at(2);
		}

	}

	// step2：对稀疏矩阵进行预分解
	SparseLU<SparseMatrix<double>> solver;
	solver.compute(A_sparse);
	if (solver.info() != Success) {
		qDebug() << "decomposition failed" << endl;
	}


	// step4：求解方程
	X_x = solver.solve(B_x);
	X_y = solver.solve(B_y);
	X_z = solver.solve(B_z);

	// step5：利用求得的X更新mesh中的所有顶点坐标
	for (size_t i = 0; i < nV; i++) {
		V* v_i = heMesh->Vertices().at(i);

		cout << X_x(i) << "	" << v_i->pos.at(0) << endl;
		heMesh->Vertices()[i]->pos.at(0) = X_x(i);
		cout << X_y(i) << "	" << v_i->pos.at(1) << endl;
		heMesh->Vertices()[i]->pos.at(1) = X_y(i);
		cout << X_z(i) << "	" << v_i->pos.at(2) << endl<< endl;
		heMesh->Vertices()[i]->pos.at(2) = X_z(i);

	}
}
